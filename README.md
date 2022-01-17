# VideoCompositing
The code that I am writing as a research assistant for Dr. Fagg at the University of Oklahoma. <br>
The point of the program is to record video from three different cameras, composit them into one frame with a resolution of 1280x960 and at 25FPS on a raspberry pi4. <br>
The program is set up to record individual frames as fast as possible and write them to disk and then a seperate program that is run after the initial data collecting program will automatically take the frames and correct their distortion (the 170degree FOV camera frames are extremely distorted), combine the images into a video and add audio. <br>
The hardware used for this project is two 170degree fov ELP Cameras, a 120FOV spedal camera, a raspberry pi4, and an external USB SSD. <br>
<br>
Note: the development of this code is currently being done using SVN. I am now only periodically uploading large changes to Github so that I have an easy way of accessing them without the SVN and so others can easily view it. <br>
Additionally, the code in the SVN repository has been integrated with the rest of the project so I can't really push to GitHub anymore. However, I have made the code compatible with Huddly cameras, I take in input from shared memory, I have made it much faster and more reliable, the cameras automatically reconnect if they drop out and much more. <br>
The completed code is currently being used with the robots at the Children's Hostpital of Pennslyvania. <br>
Feel free to send me an email if you have any questions or want to see the completed code. <br>
Contact: reza.j.torbati-1@ou.edu
