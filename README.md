# gst-streaming
The gstreamer based streaming framework.

## Software and hardware accelerated encoding
The comparison of encoding performance using H.264 and VP8 on platforms:

   * Raspberry Pi 2	(900MHz Quad-Core ARM Cortex-A7)
   * Jetson TK1		(Quad-Core ARM Cortex-A15 2.3 GHz + 192 CUDA cores)
   * MSI GS60 laptop	(Intel i7-4710HQ 2.5GHz 8 core + Nvidia GTX970M with 1280 CUDA cores)

The dataset used:
ftp://vqeg.its.bldrdoc.gov/HDTV/NTIA_source/

Used encoders:

   * Jetson TK1
     * x264enc			-	H.264 software encoder
     * omxh264enc		-	H.264 hardware accelerated encoder
     * vp8enc			-	VP8 software encoder
     * omxvp8enc		-	VP8 hardware accelerated encoder
   * Raspberry Pi 2
     * x264enc			-	H.264 software encoder
     * omxh264enc		-	H.264 hardware accelerated encoder
     * vp8enc			-	VP8 software encoder
   * MSI GS60 laptop
     * x264enc			-	H.264 software encoder
     * NVIDIA CODEC SDK	-	H.264 hardware accelerated encoder
     * vp8enc			-	VP8 software encoder

![Pipelines](https://github.com/karolmajek/gst-streaming/blob/master/images/gst-pipelines.png)

### Results
The results are shown in the table below. The values are in fps.

![Results](https://github.com/karolmajek/gst-streaming/blob/master/images/table.png)

## Point cloud visualization streaming
