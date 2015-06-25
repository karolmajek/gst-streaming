#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <pthread.h>
#include <ctime>
#include <iostream>
#include <vector>
#include <fstream>

//gst
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <cstdio>
#include <cstdlib>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;
using namespace std;

#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>
 
#define BUFLEN 512  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data
 
static int width=1280;
static int height=720;
unsigned char *buff;

GstBuffer *buffer=0;

string udp_video_ip="127.0.0.1";

float matrix[16];

vector<float> data_xyz;
vector<float> data_rgb;

bool buff_send=1;

GLuint BuildShaderProgram(const char *vsPath, const char *fsPath);
GLuint CreateShader(GLenum eShaderType, const char *strShaderFile);

void* mainGL(void*)
{


	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		exit(-1);

	SDL_Window *window = SDL_CreateWindow("OpenGL window",
	SDL_WINDOWPOS_UNDEFINED,
	SDL_WINDOWPOS_UNDEFINED,
	width, height, /*SDL_WINDOW_SHOWN | */SDL_WINDOW_OPENGL);

	SDL_GLContext glContext = SDL_GL_CreateContext(window);
	if (glContext == NULL)
	{
		printf("There was an error creating the OpenGL context!\n");
		exit(-1);
	}

	const unsigned char *version = glGetString(GL_VERSION);
	if (version == NULL) 
	{
		printf("There was an error creating the OpenGL context!\n");
		exit(-1);
	}

	SDL_GL_MakeCurrent(window, glContext);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	//MUST make a context AND make it current BEFORE glewInit()!
	glewExperimental = GL_TRUE;
	GLenum glew_status = glewInit();
	if (glew_status != 0) 
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
		exit(-1);
	}

	// Dark blue background
	glClearColor(1,1,1,0);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = BuildShaderProgram( "TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader" );

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*data_xyz.size(), data_xyz.data(), GL_STATIC_DRAW);

	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*data_rgb.size(), data_rgb.data(), GL_STATIC_DRAW);

	unsigned int color_tex;
	unsigned int fb,depth_rb;
	//RGBA8 2D texture, 24 bit depth texture, 256x256
	glGenTextures(1, &color_tex);
	glBindTexture(GL_TEXTURE_2D, color_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//NULL means reserve texture memory, but texels are undefined
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
	//-------------------------
	glGenFramebuffersEXT(1, &fb);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
	//Attach 2D texture to this FBO
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, color_tex, 0);
	//-------------------------
	glGenRenderbuffersEXT(1, &depth_rb);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth_rb);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, width, height);
	//-------------------------
	//Attach depth buffer to FBO
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depth_rb);
	//-------------------------
	//Does the GPU support current FBO configuration?
	GLenum status;
	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch(status)
	{
		case GL_FRAMEBUFFER_COMPLETE_EXT:
			break;
		default:
			printf("ERROR - FramebufferStatus\n");
			exit(-1);
	}

	glPointSize(1);

	char bGameLoopRunning = 1;
	while (bGameLoopRunning)
	{
		SDL_Event e;
		if ( SDL_PollEvent(&e) ) 
		{
			if (e.type == SDL_QUIT)
			bGameLoopRunning = 0;
			else if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE)
			bGameLoopRunning = 0;
		}
		
		double elapsed_secs;
		clock_t begin = clock();

		//-------------------------
		//and now you can render to GL_TEXTURE_2D
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);


		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		glm::mat4 Projection = glm::perspective(45.0f, 16.0f/9.0f, 0.1f, 10000.0f);

		static float angle=1;

		// Camera matrix
		glm::mat4 View       = glm::lookAt(
						glm::vec3(4,3,-3), // Camera is at (4,3,-3), in World Space
						glm::vec3(0,0,0), // and looks at the origin
						glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
				   );
		angle+=1;
		glm::mat4 rot=glm::rotate(angle,0.0f,0.0f,1.0f);
		// Model matrix : an identity matrix (model will be at the origin)
		//glm::mat4 Model      = glm::mat4(1.0f);
		// Our ModelViewProjection : multiplication of our 3 matrices
		glm::mat4 MVP        = Projection * View * rot; // Remember, matrix multiplication is the other way around


		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		//glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, matrix);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_POINTS, 0, data_xyz.size()/3);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		SDL_GL_SwapWindow(window);

		glReadPixels(0,0,width,height,GL_RGBA,GL_UNSIGNED_BYTE,buff);

		buff_send=0;
		
		while(!buff_send)
		{
			SDL_Delay(20);
		}

		elapsed_secs = (double)(clock() - begin) / CLOCKS_PER_SEC;
		static int aa=0;
		static float tt=0;
		tt+=elapsed_secs;
		if(++aa>30)
		{
			printf("FPS= %lf\n",1.0/(tt/aa)); fflush(stdout);
			aa=0;
			tt=0;
		}
	} 

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	SDL_GL_DeleteContext(glContext);
	SDL_Quit();
	return 0;
}

GLuint BuildShaderProgram(const char *vsPath, const char *fsPath)
{
	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint tempProgram;
	printf("shader: %s\n",vsPath);
	vertexShader = CreateShader(GL_VERTEX_SHADER, vsPath);
	printf("shader: %s\n",fsPath);
	fragmentShader = CreateShader(GL_FRAGMENT_SHADER, fsPath);
	printf("glCreateProgram\n");
	tempProgram = glCreateProgram();
	
	glAttachShader(tempProgram, vertexShader);
	glAttachShader(tempProgram, fragmentShader);
	
	glLinkProgram(tempProgram); //linking!
	
	//error checking
	GLint status;
    	glGetProgramiv(tempProgram, GL_LINK_STATUS, &status);
    	if (status == GL_FALSE)
    	{
    		GLint infoLogLength;
        	glGetProgramiv(tempProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
	
		GLchar strInfoLog[4096];
		glGetProgramInfoLog(tempProgram, infoLogLength, NULL, strInfoLog);
		printf("Shader linker failure: %s\n", strInfoLog);
		return -1;
	}
	else
		puts("Shader linked sucessfully!");
	
	glDetachShader(tempProgram, vertexShader);
	glDetachShader(tempProgram, fragmentShader);
	
	return tempProgram;
}

GLuint CreateShader(GLenum eShaderType, const char *strShaderFile)
{
	char shaderSource[4096];
	char inChar;
	FILE *shaderFile;
	int i = 0;
	
	shaderFile = fopen(strShaderFile, "r");
	while(fscanf(shaderFile,"%c",&inChar) > 0)
	{
		shaderSource[i++] = inChar; //loading the file's chars into array
	}
	shaderSource[i - 1] = '\0';
	fclose(shaderFile);
	//puts(shaderSource); //print to make sure the string is loaded
	
	GLuint shader = glCreateShader(eShaderType);
	const char *ss = shaderSource;
	glShaderSource(shader, 1, &ss, NULL);
	
	glCompileShader(shader);
	
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
        	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
        
        	GLchar strInfoLog[4096];
        	glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);
        
		char strShaderType[16];
		switch(eShaderType)
		{
			case GL_VERTEX_SHADER: sprintf(strShaderType, "vertex"); break;
			case GL_GEOMETRY_SHADER: sprintf(strShaderType, "geometry"); break;
			case GL_FRAGMENT_SHADER: sprintf(strShaderType, "fragment"); break;
		}

		printf("Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);
		return -1;
	}
	else
		puts("Shader compiled sucessfully!");

	return shader;
}


static GMainLoop *loop;


    static void
cb_need_data (GstAppSrc *appsrc,
        guint       unused_size,
        gpointer    user_data)
{
	printf("#");
	buff_send=1;
	
	
	static clock_t begin = clock();
    static GstClockTime timestamp = 0;
    guint size;
    GstFlowReturn ret;

	size = width*height * 4;
	//pthread_mutex_lock (&mutex_buff);
	gst_buffer_fill(buffer,0,buff,size);
	//pthread_mutex_unlock (&mutex_buff);

	GST_BUFFER_PTS (buffer) = timestamp;

	timestamp=GST_SECOND * double(clock() - begin) / CLOCKS_PER_SEC;


    g_signal_emit_by_name (appsrc, "push-buffer", buffer, &ret);
    //ret = gst_app_src_push_buffer(appsrc, buffer);

    if (ret != GST_FLOW_OK) {
        /* something wrong, stop pushing */
        g_main_loop_quit (loop);
    }
}
static void cb_enough_data(GstAppSrc *src, gpointer user_data)
{
    g_print("In %s\n", __func__);
}

static gboolean cb_seek_data(GstAppSrc *src, guint64 offset, gpointer user_data)
{
    g_print("In %s\n", __func__);
    return TRUE;
}
void* mainGST (void* argv)
{
	GError *error = NULL;
    GstElement /*pipeline,*/ *appsrc, *conv, *parse, *payloader, *udpsink, *videoenc;
    //GstElement *v4l2src, *conv1, *appsink;

    /* init GStreamer */
    gst_init (0, 0);
    loop = g_main_loop_new (NULL, FALSE);

	guint size;

	size = width*height * 4;
	buffer = gst_buffer_new_allocate (NULL, size, NULL);
	
    /* setup pipeline */
    /*pipeline = gst_pipeline_new ("pipeline");
    appsrc = gst_element_factory_make ("appsrc", "source");
    conv = gst_element_factory_make ("videoconvert", "conv");
    videoenc = gst_element_factory_make("omxh264enc", "videoenc");
    parse = gst_element_factory_make("h264parse", "parse");
    payloader = gst_element_factory_make("rtph264pay", "payloader");
    udpsink = gst_element_factory_make("udpsink", "udpsink");
    */
    
    printf("Line %d\n",__LINE__);
    
	gchar *descr = g_strdup(
	"appsrc name=source ! "
	"videoconvert name=conv ! "
	"omxh264enc name=videoenc ! "
	"h264parse name=parse ! "
	"rtph264pay name=payloader ! "
	"udpsink name=udpsink"
	);
	
    printf("Line %d\n",__LINE__);
	GstElement *pipeline = gst_parse_launch (descr, &error);

    printf("Line %d\n",__LINE__);
	if (error != NULL)
	{
		g_print ("could not construct pipeline: %s\n", error->message);
		g_error_free (error);
		exit (-1);
	}
    printf("Line %d\n",__LINE__);
    
	appsrc = gst_bin_get_by_name (GST_BIN (pipeline), "source");
	conv = gst_bin_get_by_name (GST_BIN (pipeline), "conv");
	videoenc = gst_bin_get_by_name (GST_BIN (pipeline), "videoenc");
	parse = gst_bin_get_by_name (GST_BIN (pipeline), "parse");
	payloader = gst_bin_get_by_name (GST_BIN (pipeline), "payloader");
	udpsink = gst_bin_get_by_name (GST_BIN (pipeline), "udpsink");
    
    
    printf("Line %d\n",__LINE__);
    

    /* setup */
    g_object_set(G_OBJECT(payloader),
            "config-interval", 1,
            NULL);
    printf("Line %d\n",__LINE__);
    g_object_set(G_OBJECT(udpsink),
            "host", udp_video_ip.c_str(),
            "port", 5000,
            NULL);
    printf("Line %d\n",__LINE__);
    g_object_set (G_OBJECT (appsrc), "caps",
            gst_caps_new_simple ("video/x-raw",
                "format", G_TYPE_STRING, "RGBA",
                "width", G_TYPE_INT, width,
                "height", G_TYPE_INT, height,
                "pixel-aspect-ratio", GST_TYPE_FRACTION, 1, 1,
                "framerate", GST_TYPE_FRACTION, 0, 1,
                NULL), NULL);

    printf("Line %d\n",__LINE__);
   // gst_bin_add_many (GST_BIN (pipeline), appsrc, conv, videoenc, parse, payloader, udpsink, NULL);
    //gst_element_link_many (appsrc, conv, videoenc, parse, payloader, udpsink, NULL);

    /* setup appsrc */
    g_object_set (G_OBJECT (appsrc),
            "stream-type", 0,
            "is-live", TRUE,
            "format", GST_FORMAT_TIME, NULL);
    GstAppSrcCallbacks cbs;
    cbs.need_data = cb_need_data;
    cbs.enough_data = cb_enough_data;
    cbs.seek_data = cb_seek_data;
    //g_signal_connect (appsrc, "need-data", G_CALLBACK (cb_need_data), NULL);
    gst_app_src_set_callbacks(GST_APP_SRC_CAST(appsrc), &cbs, NULL, NULL);

    /* play */
    gst_element_set_state (pipeline, GST_STATE_PLAYING);
    g_main_loop_run (loop);

    /* clean up */
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (GST_OBJECT (pipeline));
    g_main_loop_unref (loop);

    return 0;
}


void die(char *s)
{
    perror(s);
    exit(1);
}
 
void* server_main(void*)
{
    struct sockaddr_in si_me, si_other;
     
    int s, i, slen = sizeof(si_other) , recv_len;
    char buf[BUFLEN];
     
    //create a UDP socket
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
     
    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));
     
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
     
    //bind socket to port
    if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        die("bind");
    }
     
    //keep listening for data
    while(1)
    {
        //printf("Waiting for data...");
        fflush(stdout);
         
        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, (socklen_t*)&slen)) == -1)
        {
            die("recvfrom()");
        }
         
        //print details of the client/peer and the data received
        //printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        
        int size=16*sizeof(float);
        if(recv_len==size)
		{
			memcpy(matrix,buf,size);
			
		}
    }
 
    //close(s);
    return 0;
}


int main(int argc, char **argv)
{
	printf("\n------------------------------------------------\nUsage: %s 127.0.0.1\n",argv[0]);
	if(argc==2)
	{
		udp_video_ip=argv[1];
	}
	
	printf("Will send the H264 RTP video to %s\n------------------------------------------------\n\n",udp_video_ip.c_str());
	
	
	ifstream f("scan000.3d");
	printf("Loading file %s\n","scan000.3d");
	if(f.good())
	{
		while(!f.eof())
		{
			string s;
			getline(f,s);
			
			float x,y,z;
			int r,g,b;
			sscanf(s.c_str(),"%f %f %f %d %d %d",&x,&y,&z,&r,&g,&b);
			
			/*
			//Load cloud in [m] with Z up
			data_xyz.push_back(x);
			data_xyz.push_back(y);
			data_xyz.push_back(z);
			//Load cloud in [m] with Z up
			*/
			
			//Load cloud in [mm] with Y up
			data_xyz.push_back(x/1000.0f);
			data_xyz.push_back(-z/1000.0f);
			data_xyz.push_back(y/1000.0f);
			//Load cloud in [mm] with Y up
			
			
			data_rgb.push_back(r/255.0);
			data_rgb.push_back(g/255.0);
			data_rgb.push_back(b/255.0);
			
			
			if(!((int(data_xyz.size()/3))%100000))
			{
				printf("Loaded %d points  |%s|\n",data_xyz.size()/3,s.c_str());
				fflush(stdout);
			}
		}
		
		f.close();
	}
	
	printf("Loaded %d points\n",data_xyz.size()/3);
	
	if(!data_xyz.size())	//no points? -> generate
	{
		for(int i=0;i<1000;++i)
		{
			data_xyz.push_back((rand()%100)/10.0f);
			data_xyz.push_back((rand()%100)/10.0f);
			data_xyz.push_back((rand()%100)/10.0f);
			
			data_rgb.push_back((rand()%101)/100.0f);
			data_rgb.push_back((rand()%101)/100.0f);
			data_rgb.push_back((rand()%101)/100.0f);
		}
		printf("Generated %d points\n",data_xyz.size()/3);
	}
	
	
	buff=new unsigned char [width*height*4];
	for(int i=0;i<width*height;++i)
	{
		buff[4*i+0]=rand()%256;
		buff[4*i+1]=rand()%256;
		buff[4*i+2]=rand()%256;
		buff[4*i+3]=255;
	}
  
  
     pthread_t thread1, thread2, server_thread;
     
     int  ret;

    /* Create independent threads each of which will execute function */

     ret = pthread_create( &thread1, NULL, mainGL, (void *)argv);
     if(ret)
     {
         fprintf(stderr,"Error - pthread_create(mainGL) return code: %d\n",ret);
         exit(EXIT_FAILURE);
     }
     ret = pthread_create( &thread2, NULL, mainGST, (void *)argv);
     if(ret)
     {
         fprintf(stderr,"Error - pthread_create(mainGST) return code: %d\n",ret);
         exit(EXIT_FAILURE);
     }
     ret = pthread_create( &server_thread, NULL, server_main, (void *)argv);
     if(ret)
     {
         fprintf(stderr,"Error - pthread_create(server_main) return code: %d\n",ret);
         exit(EXIT_FAILURE);
     }



     /* Wait till threads are complete before main continues. Unless we  */
     /* wait we run the risk of executing an exit which will terminate   */
     /* the process and all threads before the threads have completed.   */

     pthread_join( server_thread, NULL); 
     pthread_join( thread1, NULL);
     pthread_join( thread2, NULL); 

     exit(EXIT_SUCCESS);
}
