# The Detection Algorithm

In two Parts
first show the Flowchart then describe every action

Why Contrast?
More robust when the Lighting Changes
-> Possible explanation : 
Contrast is the Ratio of Photons of the Background to the Foreground
This wont change with the amount of photons hitting the Plate

Problems
Glare in the B-Channel -> not very sharp images

1. Grab Image ~100 ms
   1. We also load the flatfield which is an already taken image
      1. The flatfield is an image with nothing on it
2. Remove the Vignette ~30ms
   1. Convert the images to HSV
   2. Divide the V channel of the image by the V channel of the flatfield
      1. The vignette only manifests in the V Channel
   3. Multiply the image V channel by the mean of the flatfield
   4. The vignette and all artifacts of the lens and camera are now removed
3. Mask out the Background ~15ms
   1. Create a colorhistogram of the image
   2. Calculate the maximum of each colorhistogram in RGB space
      1. This is the background value, as the background is homogenious and takes the most space
    1. Define all colorvalues within a certain margin around the maximum as background values
    2. Mask these values in the image
    3. The backgrund is now masked and you have the mean background value which is defined as the maximum of the histogram
4. Calculate the Contrast of each Pixel ~50ms
   1. We calculate the Weber contrast of each pixel relative to the mean background values
   2. Save the contrasts in a 3D array
5. Mask the the Contrasts with specific Values ~20ms
   1. The contrasts of the flakes follow a gaussian distribution
      1. e.g. die contrast of monolayer graphene in the R-channel is 0.12+/-0.04
   2. Look for contrasts within an ellipsoid in the 3D contrast space
      1. Center and radii of the ellipsoid are predefined my us
6. Clean the Mask and Label each blob on the mask < 5ms
   1. The cleaning are just a few morph operations
   2. We are running a blobfinding algorithm
7. Filter Small Blobs < 5ms
   1. These are possibly dirt or other impurities

Has a Flake been detected?

No:

Get the next Image

Yes:

1. Run Heavy Metrics on each found Flake
   1. We can now run extremly heavy metrics on these images as this has minimal impact on the final duration of the full scan.
2. Finished, A Flake has been detected!