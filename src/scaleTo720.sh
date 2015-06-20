gst-launch-1.0 filesrc location=../media/Aspen_8bit.avi ! avidemux ! videoconvert ! videoscale ! video/x-raw,width=1280,height=720 ! avimux ! filesink location=../media/Aspen720.avi
gst-launch-1.0 filesrc location=../media/ControlledBurn_8bit.avi ! avidemux ! videoconvert ! videoscale ! video/x-raw,width=1280,height=720 ! avimux ! filesink location=../media/ControlledBurn720.avi
gst-launch-1.0 filesrc location=../media/RedKayak_8bit.avi ! avidemux ! videoconvert ! videoscale ! video/x-raw,width=1280,height=720 ! avimux ! filesink location=../media/RedKayak720.avi
gst-launch-1.0 filesrc location=../media/RushFieldCuts_8bit.avi ! avidemux ! videoconvert ! videoscale ! video/x-raw,width=1280,height=720 ! avimux ! filesink location=../media/RushFieldCuts720.avi
gst-launch-1.0 filesrc location=../media/SnowMnt_8bit.avi ! avidemux ! videoconvert ! videoscale ! video/x-raw,width=1280,height=720 ! avimux ! filesink location=../media/SnowMnt720.avi
gst-launch-1.0 filesrc location=../media/SpeedBag_8bit.avi ! avidemux ! videoconvert ! videoscale ! video/x-raw,width=1280,height=720 ! avimux ! filesink location=../media/SpeedBag720.avi
gst-launch-1.0 filesrc location=../media/TouchdownPass_8bit.avi ! avidemux ! videoconvert ! videoscale ! video/x-raw,width=1280,height=720 ! avimux ! filesink location=../media/TouchdownPass720.avi
gst-launch-1.0 filesrc location=../media/WestWindEasy_8bit.avi ! avidemux ! videoconvert ! videoscale ! video/x-raw,width=1280,height=720 ! avimux ! filesink location=../media/WestWindEasy720.avi
