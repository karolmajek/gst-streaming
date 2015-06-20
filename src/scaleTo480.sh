gst-launch-1.0 filesrc location=../media/Aspen_8bit.avi ! avidemux ! videoconvert ! videoscale ! video/x-raw,width=854,height=480 ! avimux ! filesink location=../media/Aspen480.avi
gst-launch-1.0 filesrc location=../media/ControlledBurn_8bit.avi ! avidemux ! videoconvert ! videoscale ! video/x-raw,width=854,height=480 ! avimux ! filesink location=../media/ControlledBurn480.avi
gst-launch-1.0 filesrc location=../media/RedKayak_8bit.avi ! avidemux ! videoconvert ! videoscale ! video/x-raw,width=854,height=480 ! avimux ! filesink location=../media/RedKayak480.avi
gst-launch-1.0 filesrc location=../media/RushFieldCuts_8bit.avi ! avidemux ! videoconvert ! videoscale ! video/x-raw,width=854,height=480 ! avimux ! filesink location=../media/RushFieldCuts480.avi
gst-launch-1.0 filesrc location=../media/SnowMnt_8bit.avi ! avidemux ! videoconvert ! videoscale ! video/x-raw,width=854,height=480 ! avimux ! filesink location=../media/SnowMnt480.avi
gst-launch-1.0 filesrc location=../media/SpeedBag_8bit.avi ! avidemux ! videoconvert ! videoscale ! video/x-raw,width=854,height=480 ! avimux ! filesink location=../media/SpeedBag480.avi
gst-launch-1.0 filesrc location=../media/TouchdownPass_8bit.avi ! avidemux ! videoconvert ! videoscale ! video/x-raw,width=854,height=480 ! avimux ! filesink location=../media/TouchdownPass480.avi
gst-launch-1.0 filesrc location=../media/WestWindEasy_8bit.avi ! avidemux ! videoconvert ! videoscale ! video/x-raw,width=854,height=480 ! avimux ! filesink location=../media/WestWindEasy480.avi
