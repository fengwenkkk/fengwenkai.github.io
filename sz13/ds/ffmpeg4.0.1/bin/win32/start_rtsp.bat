rem ffmpeg -i e:\iceland.mp4 -vcodec libx264 -f rtsp rtsp://127.0.0.1:554/test

ffmpeg -re -i "e:\test_1080.wmv" -vcodec copy -f rtsp -rtsp_transport tcp rtsp://127.0.0.1:554/test.sdp 

rem ffmpeg -re -i e:\iceland.mp4 -vcodec libx264 -acodec copy -f rtsp rtsp://127.0.0.1:554/test.sdp