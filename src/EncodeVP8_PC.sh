echo Software encoding 1080p sources
gst-launch-1.0 filesrc location=../media/Aspen_8bit.avi ! avidemux ! videoconvert ! progressreport ! vp8enc threads=8 ! matroskamux ! filesink location=../media/Aspen1080.mkv
gst-launch-1.0 filesrc location=../media/ControlledBurn_8bit.avi ! avidemux ! videoconvert ! progressreport ! vp8enc threads=8 ! matroskamux ! filesink location=../media/ControlledBurn1080.mkv
gst-launch-1.0 filesrc location=../media/RedKayak_8bit.avi ! avidemux ! videoconvert ! progressreport ! vp8enc threads=8 ! matroskamux ! filesink location=../media/RedKayak1080.mkv
gst-launch-1.0 filesrc location=../media/RushFieldCuts_8bit.avi ! avidemux ! videoconvert ! progressreport ! vp8enc threads=8 ! matroskamux ! filesink location=../media/RushFieldCuts1080.mkv
gst-launch-1.0 filesrc location=../media/SnowMnt_8bit.avi ! avidemux ! videoconvert ! progressreport ! vp8enc threads=8 ! matroskamux ! filesink location=../media/SnowMnt1080.mkv
gst-launch-1.0 filesrc location=../media/SpeedBag_8bit.avi ! avidemux ! videoconvert ! progressreport ! vp8enc threads=8 ! matroskamux ! filesink location=../media/SpeedBag1080.mkv
gst-launch-1.0 filesrc location=../media/TouchdownPass_8bit.avi ! avidemux ! videoconvert ! progressreport ! vp8enc threads=8 ! matroskamux ! filesink location=../media/TouchdownPass1080.mkv
gst-launch-1.0 filesrc location=../media/WestWindEasy_8bit.avi ! avidemux ! videoconvert ! progressreport ! vp8enc threads=8 ! matroskamux ! filesink location=../media/WestWindEasy1080.mkv
echo Software encoding 1080p sources	DONE
echo Software encoding 720p sources
gst-launch-1.0 filesrc location=../media/Aspen720.avi ! avidemux ! videoconvert ! progressreport ! vp8enc threads=8 ! matroskamux ! filesink location=../media/Aspen720.mkv
gst-launch-1.0 filesrc location=../media/ControlledBurn720.avi ! avidemux ! videoconvert ! progressreport ! vp8enc threads=8 ! matroskamux ! filesink location=../media/ControlledBurn720.mkv
gst-launch-1.0 filesrc location=../media/RedKayak720.avi ! avidemux ! videoconvert ! progressreport ! vp8enc threads=8 ! matroskamux ! filesink location=../media/RedKayak720.mkv
gst-launch-1.0 filesrc location=../media/RushFieldCuts720.avi ! avidemux ! videoconvert ! progressreport ! vp8enc threads=8 ! matroskamux ! filesink location=../media/RushFieldCuts720.mkv
gst-launch-1.0 filesrc location=../media/SnowMnt720.avi ! avidemux ! videoconvert ! progressreport ! vp8enc threads=8 ! matroskamux ! filesink location=../media/SnowMnt720.mkv
gst-launch-1.0 filesrc location=../media/SpeedBag720.avi ! avidemux ! videoconvert ! progressreport ! vp8enc threads=8 ! matroskamux ! filesink location=../media/SpeedBag720.mkv
gst-launch-1.0 filesrc location=../media/TouchdownPass720.avi ! avidemux ! videoconvert ! progressreport ! vp8enc threads=8 ! matroskamux ! filesink location=../media/TouchdownPass720.mkv
gst-launch-1.0 filesrc location=../media/WestWindEasy720.avi ! avidemux ! videoconvert ! progressreport ! vp8enc threads=8 ! matroskamux ! filesink location=../media/WestWindEasy720.mkv
echo Software encoding 720p sources	DONE
echo Software encoding 480p sources
gst-launch-1.0 filesrc location=../media/Aspen480.avi ! avidemux ! videoconvert ! progressreport ! vp8enc threads=8 ! matroskamux ! filesink location=../media/Aspen480.mkv
gst-launch-1.0 filesrc location=../media/ControlledBurn480.avi ! avidemux ! videoconvert ! progressreport ! vp8enc threads=8 ! matroskamux ! filesink location=../media/ControlledBurn480.mkv
gst-launch-1.0 filesrc location=../media/RedKayak480.avi ! avidemux ! videoconvert ! progressreport ! vp8enc threads=8 ! matroskamux ! filesink location=../media/RedKayak480.mkv
gst-launch-1.0 filesrc location=../media/RushFieldCuts480.avi ! avidemux ! videoconvert ! progressreport ! vp8enc threads=8 ! matroskamux ! filesink location=../media/RushFieldCuts480.mkv
gst-launch-1.0 filesrc location=../media/SnowMnt480.avi ! avidemux ! videoconvert ! progressreport ! vp8enc threads=8 ! matroskamux ! filesink location=../media/SnowMnt480.mkv
gst-launch-1.0 filesrc location=../media/SpeedBag480.avi ! avidemux ! videoconvert ! progressreport ! vp8enc threads=8 ! matroskamux ! filesink location=../media/SpeedBag480.mkv
gst-launch-1.0 filesrc location=../media/TouchdownPass480.avi ! avidemux ! videoconvert ! progressreport ! vp8enc threads=8 ! matroskamux ! filesink location=../media/TouchdownPass480.mkv
gst-launch-1.0 filesrc location=../media/WestWindEasy480.avi ! avidemux ! videoconvert ! progressreport ! vp8enc threads=8 ! matroskamux ! filesink location=../media/WestWindEasy480.mkv
echo Software encoding 480p sources	DONE
