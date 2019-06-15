# LabelMe

   *LabelMe* is a application to labeling, reviewing and editing image datasets.
   This application has two section, **"CROP ROI"** to labeling images and **"REVIEW&EDIT"** to reviewing results.
   
 Requirements:  
- Just install [**Qt5**](https://wiki.qt.io/Install_Qt_5_on_Ubuntu "Install Qt 5 on Ubuntu").

Usage:

 - To add new ROI on image, first drew an ROI then insert the abbreviation name of class that you have already defined it, then press the Enter key.

Note that:
  - It is not necessary to type the abbreviation name if you have only one class.
  - If drawned ROI was out of image, it is modified to image boundary automatically.
  - If everything was correct you show result on image and you can submit it else you must correct the mistake.
  - You can use the undo and redo keys.
  - In Review&Edit mode for delete an ROI just double-click inside the area.
  - To drew an ROI In rectangle (default) mode, press the left mouse button at one corner of the box, hold the button down and move the mouse to the desired position of the other corner and release.
  - To drew an ROI In polygon mode, first click on desired point as beginning point, then press and hold down and move the mouse to drew a line of polygon, the release it, do this until complete the polygon.
  - In the same imgae you can drew rectangle and polygon ROIs simultaneously.
  - You can zoom on image by mouse scroll.
  - If you close LabelMe before label all of the images, in CROP ROI mode, next time to countinue  from last state, at Login Window just select the previous result text file as store text file. In REVIEW&EDIT mode you can use from image name Line Edite section on top left of image view.
 - When you have many classes, you can write class names in a text file and use it next times. Note that every line must begin with "#class_names:" like: #class_names: [class full name] [abbreviation name] [color code (optional)] `(#class_names: body b #ff007f)`

- Also you can use the store text file to upload as class names file.

- You can see the Log file to view your performance. :)
