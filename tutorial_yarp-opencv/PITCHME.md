#HSLIDE

### Robot vision tutorial with OpenCV

#HSLIDE

#### YARP & OPENCV

#HSLIDE
### Goals of this Tutorial
 - Track something <span style="color:#e49436">round</span> and <span style="color:#e49436">red</span> :-)
 - integrating <span style="color:#e49436">YARP</span> with <span style="color:#e49436">OpenCV</span> while getting
 <span style="color:#e49436">live</span> image <span style="color:#e49436">streams</span>.
 - yarp::os::<span style="color:#e49436">RFModule</span> with port <span style="color:#e49436">Callbacks</span>
 - <span style="color:#e49436">Thrift</span> services
 - performing simple <span style="color:#e49436">image processing</span> operations

#VSLIDE
### Let's plan what to do...
 - <span style="color:#e49436">Receive</span> a stream of images from a port
 - Use some <span style="color:#e49436">image processing</span> techniques to make things easier.
 - Display it: <span style="color:#e49436">stream</span> it through a <span style="color:#e49436">yarp port</span> to a <span style="color:#e49436">yarpviewer</span>.
 - Modify the streamed image to <span style="color:#e49436">display</span> the <span style="color:#e49436">location</span> of the red and round object.
 
#HSLIDE
### YARP & OPENCV
@fa[arrow-down]

#VSLIDE

@snap[north]
#### YARP & OPENCV
@snapend

@snap[text-left]
@ul[list-spaced-bullets text-07](false)
- Convert yarp image to <span style="color:#e49436">`cv::Mat`</span>:
@ulend
@snapend

@snap[span-100 text-07]
```c++
cv::Mat in_cv = yarp::cv::toCvMat(outImage);
```
@snapend

@snap[text-left]
@ul[list-spaced-bullets text-07](false)
- Apply opencv functions:
@ulend
@snapend

@snap[span-100 text-07]
```c++
cv::GaussianBlur(redBallOnly, redBallOnly, cv::Size(gausian_size, gausian_size), 2, 2);

cv::dilate(redBallOnly, redBallOnly, cv::Mat(), cv::Point(-1,-1), dilate_niter, cv::BORDER_CONSTANT, cv::morphologyDefaultBorderValue());
        
cv::erode(redBallOnly, redBallOnly, cv::Mat(), cv::Point(-1,-1), erode_niter, cv::BORDER_CONSTANT, cv::morphologyDefaultBorderValue());
```
@snapend

#HSLIDE
### Some Image Processing techniques
@fa[arrow-down]

#VSLIDE

@snap[north span-80]
### Smoothing
@img[](tutorial_yarp-opencv/images/smoothing.png)
@snapend

@snap[west span-40]
@ul[list-spaced-bullets text-left text-07](false)
- To reduce noise (high frequency content)
- Gaussian filter most used:
@ulend
@snapend

@snap[midpoint span-20 text-center text-06]
@img[](tutorial_yarp-opencv/images/original.png)
Before smoothing
@snapend

@snap[east span-20 text-center text-06]
@img[](tutorial_yarp-opencv/images/blurred.png)
After smoothing
@snapend

#VSLIDE
@snap[north span-100]
### Morphological operations
@snapend

@snap[west span-45 text-center]
Dilation:
@img[](tutorial_yarp-opencv/images/dilation.gif)
@img[](tutorial_yarp-opencv/images/dilation.png)
@snapend

@snap[east span-45 text-center]
Erosion:
@img[](tutorial_yarp-opencv/images/erosion.gif)
@img[](tutorial_yarp-opencv/images/erosion.png)
@snapend

#VSLIDE
@snap[north text-left span-100]
### Circular Hough transform
@snapend

@snap[west span-45 text-center text-07]
One circle with fixed radius:
@img[](tutorial_yarp-opencv/images/cht.png)
More circles with fixed radius:
@img[](tutorial_yarp-opencv/images/cht-morecircles.png)
@snapend

@snap[east span-25 text-center text-07]
Unknown radius:
@img[](tutorial_yarp-opencv/images/cht-radii.png)
@snapend

#HSLIDE
### Draw and display results
@fa[arrow-down]

#VSLIDE
### Draw and display results

@snap[span-100 text-07]
```c++
for (size_t i = 0; i < circles.size(); i++)
{
    cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
    int radius = cvRound(circles[i][2]);
    // circle center
    circle(in_cv, center, 3, cv::Scalar(0, 255, 0), -1, 8, 0);
    // circle outline
    circle(in_cv, center, radius, cv::Scalar(0, 0, 255), 3, 8, 0);

    yarp::os::Bottle &t=outTargets.addList();
    t.addDouble(center.x);
    t.addDouble(center.y);
}
```
@snapend

#HSLIDE
Now let's code :)
