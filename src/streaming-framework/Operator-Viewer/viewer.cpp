#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <stdlib.h>
#include <stdio.h>

#include <GL/glut.h>
#include <stdlib.h>
#include <string>
#include <sys/time.h>
#include <pthread.h>
#include <ctime>
#include <unistd.h>
#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit();
#include <arpa/inet.h>
#include <sys/socket.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
using namespace glm;
using namespace std;


#define BUFLEN 512  //Max length of buffer
#define PORT 8888   //The port on which to send data

string server_ip="127.0.0.1";
 
void die(char *s)
{
    perror(s);
    exit(1);
}
 
struct sockaddr_in si_other;
int s, i, slen=sizeof(si_other);
char buf[BUFLEN];
char message[BUFLEN];

GLuint textureID=0;
float matrix[16];

#define VIEWING_DISTANCE_MIN  0.1

float angle_x=15,angle_z=30,translate_z=-3;


static int width=1280;
static int height=720;

char* texture_buff;


bool left_button = 0;
bool right_button = 0;
GLfloat g_nearPlane = 0.01;
GLfloat g_farPlane = 100;
int posx = 0;
int posy=0;

struct timeval last_idle_time;



void display(void)
{
	glEnable(GL_TEXTURE_2D);
	glDeleteTextures(1, &textureID);
	textureID=0;
	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_buff);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	

	g_main_iteration(0);

	static double tt=0;
	static int frames=0;

	static clock_t begin = clock();
	double elapsed_secs = double(clock() - begin) / CLOCKS_PER_SEC;
	begin = clock();

	if(frames>=100)
	{
		char txt[2000];
		sprintf(txt,"OpenGL window FPS: %d / %lf = %lf\n",frames,tt,frames/tt);
		
		glutSetWindowTitle(txt);
		tt=0;
		frames=0;
	}

	tt+=elapsed_secs;
	++frames;

	// Clear frame buffer and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




	glMatrixMode(GL_MODELVIEW);
	
	
	glPushMatrix();
	
	glColor4f(1,1,1,1);
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glBegin(GL_QUADS);
	glTexCoord2f(0,1);
	glVertex3f(0, 0, 0);
	glTexCoord2f(1,1); 
	glVertex3f(1, 0, 0);
	glTexCoord2f(1,0); 
	glVertex3f(1, 1, 0);
	glTexCoord2f(0,0); 	
	glVertex3f(0, 1, 0);   	
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	// Make sure changes appear onscreen
	glutSwapBuffers();

	
}

void reshape(GLint width, GLint height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,1,0,1,-100,100);
}

void InitGraphics(void)
{
	int width, height;
	int nComponents;

	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
}

void MouseButton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		left_button = (state == GLUT_DOWN);
		posy=y;
		posx=x;
	}
	if (button == GLUT_RIGHT_BUTTON)
	{
		right_button = (state == GLUT_DOWN);
		posy=y;
		posx=x;
	}
}

void MouseMotion(int x, int y)
{
	if (left_button)
	{
		angle_z += (posx-x) / 2.0;
		angle_x += (posy-y) / 2.0;
		
		posy=y;
		posx=x;
		
	}
	if (right_button)
	{
		translate_z-=(posy-y)/10.0;
		
		posy=y;
		posx=x;
	}
	

	glm::mat4 Projection = glm::perspective(45.0f, 16.0f/9.0f, 0.1f, 5000.0f);
	
						   
	glm::mat4 tr=glm::translate(0.0f,0.0f,translate_z);
	glm::mat4 rot1=glm::rotate(angle_x,1.0f,0.0f,0.0f);
	glm::mat4 rot2=glm::rotate(angle_z,0.0f,0.0f,1.0f);
	
	glm::mat4 MVP=Projection*tr*rot1*rot2; 

	for(int i=0;i<4;++i)
	{
		for(int j=0;j<4;++j)
		{
			matrix[i*4+j]=MVP[i][j];
		}
	}

	if (sendto(s, matrix, 16*sizeof(float) , 0 , (struct sockaddr *) &si_other, slen)==-1)
	{
		die("sendto()");
	}
	
	glutPostRedisplay();
}

void AnimateScene(void)
{	
	float dt;

	// Figure out time elapsed since last call to idle function
	struct timeval time_now;
	gettimeofday(&time_now, NULL);
	dt = (float)(time_now.tv_sec  - last_idle_time.tv_sec) +
	1.0e-6*(time_now.tv_usec - last_idle_time.tv_usec);


	// Save time_now for next time
	last_idle_time = time_now;

	// Force redraw
	glutPostRedisplay();
}


void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 27:             // ESCAPE key
		exit (0);
		break;
	
	}
}



GstFlowReturn
new_preroll(GstAppSink *appsink, gpointer data)
{
	g_print ("Got preroll!\n");
	return GST_FLOW_OK;
}

GstFlowReturn new_sample(GstAppSink *appsink, gpointer gdata)
{
	static int framecount = 0;
	framecount++;

	GstSample *sample = gst_app_sink_pull_sample(appsink);
	GstCaps *caps = gst_sample_get_caps(sample);
	GstBuffer *buffer = gst_sample_get_buffer(sample);
	const GstStructure *info = gst_sample_get_info(sample);

	GstMapInfo map;
	gst_buffer_map (buffer, &map, GST_MAP_READ);
	memcpy(texture_buff,map.data,width*height*3);

	gst_buffer_unmap(buffer, &map);



	// show caps on first frame
	if (framecount == 1)
	{
		printf("################\n");
		g_print ("%s\n", gst_caps_to_string(caps));
		printf("################\n");
	}

	gst_sample_unref (sample);
	return GST_FLOW_OK;
}

static gboolean
my_bus_callback (GstBus *bus, GstMessage *message, gpointer data)
{

	switch (GST_MESSAGE_TYPE (message))
	{
		case GST_MESSAGE_ERROR: {
		GError *err;
		gchar *debug;

		gst_message_parse_error (message, &err, &debug);
		g_print ("Error: %s\n", err->message);
		g_error_free (err);
		g_free (debug);    
		break;
		}
		case GST_MESSAGE_TAG:
		{
		GstTagList *tags = NULL;

		gst_message_parse_tag (message, &tags);
		//g_print ("Got tags from element %s\n", GST_OBJECT_NAME (message->src));
		gchar* tt=gst_tag_list_to_string(tags);
		//g_print(tt);
		g_free(tt);
		gst_tag_list_free (tags);
		break;
		}
		case GST_MESSAGE_EOS:
		/* end-of-stream */
		printf("GST_MESSAGE_EOS\n");
		break;
		default:
		/* unhandled message */
		//g_print ("Got %s message\n", GST_MESSAGE_TYPE_NAME (message));
		//printf("unhandled message GST_MESSAGE_TYPE (message)=%d\n",GST_MESSAGE_TYPE (message));
		break;
	}
	/* we want to be notified again the next time there is a message
	* on the bus, so returning 1 (0 means we want to stop watching
	* for messages on the bus and our callback should not be called again)
	*/
	return 1;
}




int main(int argc, char **argv)
{
	printf("\n------------------------------------------------\nUsage: %s 127.0.0.1\n",argv[0]);
	if(argc==2)
	{
		server_ip=argv[1];
	}
	
	printf("Will send the matrices to %s:%d\n------------------------------------------------\n\n",server_ip.c_str(),PORT);
	
    if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
 
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);
     
    if (inet_aton(server_ip.c_str() , &si_other.sin_addr) == 0) 
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
    
	
	//identity matrix
	for(int i=0;i<16;++i)
	{
		matrix[i]=0;
	}
	matrix[0]=matrix[5]=matrix[10]=matrix[15]=1;
	
	
	
	GError *error = NULL;

	gst_init (&argc, &argv);

	gchar *descr = g_strdup(
	"udpsrc port=5000 caps=\"application/x-rtp, media=(string)video,clock-rate=(int)90000,encoding-name=(string)H264\" ! rtph264depay ! h264parse ! omxh264dec ! "
	"videoconvert ! "
	"video/x-raw,format=RGB ! "
	"videoconvert ! "
	"appsink name=sink sync=1   "
	);
	GstElement *pipeline = gst_parse_launch (descr, &error);

	if (error != NULL) {
	g_print ("could not construct pipeline: %s\n", error->message);
	g_error_free (error);
	exit (-1);
	}

	/* get sink */
	GstElement *sink = gst_bin_get_by_name (GST_BIN (pipeline), "sink");

	gst_app_sink_set_emit_signals((GstAppSink*)sink, 1);
	gst_app_sink_set_drop((GstAppSink*)sink, 1);
	gst_app_sink_set_max_buffers((GstAppSink*)sink, 1);
	GstAppSinkCallbacks callbacks = { NULL, new_preroll, new_sample };
	gst_app_sink_set_callbacks (GST_APP_SINK(sink), &callbacks, NULL, NULL);

	
	
	GstBus *bus;
	guint bus_watch_id;
	bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
	bus_watch_id = gst_bus_add_watch (bus, my_bus_callback, NULL);
	gst_object_unref (bus);

	gst_element_set_state (GST_ELEMENT (pipeline), GST_STATE_PLAYING);



	// GLUT Window Initialization:

	glutInit (&argc,(char**)argv);

	glutInitWindowSize (width, height);

	glutInitDisplayMode ( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

	glutCreateWindow ("GLUT");


	// Initialize OpenGL graphics state
	InitGraphics();



	texture_buff=new char[width*height*3];
	for(int i=0;i<width*height*3;++i)
	{
		texture_buff[i]=rand()%255;
	}
	
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &textureID);
	
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_buff);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	

	// Register callbacks:
	glutDisplayFunc (display);
	glutReshapeFunc (reshape);
	glutKeyboardFunc (Keyboard);
	glutMouseFunc (MouseButton);
	glutMotionFunc (MouseMotion);
	glutIdleFunc (AnimateScene);



	gettimeofday (&last_idle_time, NULL);


	// Turn the flow of control over to GLUT
	glutMainLoop ();

	gst_element_set_state (GST_ELEMENT (pipeline), GST_STATE_NULL);
	gst_object_unref (GST_OBJECT (pipeline));

	delete[] texture_buff;
	return 0;
}








