# Removing the Vignette

The Vignette only manifests in the V Channel of the Image.

Show the HSV Channels of trhe Flatfield.

Convert the Image and the Flatfield to HSV and take the V Channel.

Remove the Vigentte of the Image by applying the following formula.

$I^V_{Corrected} = \frac{I^V_{Original}}{I^V_{Flatfield}} * \bar{I}^V_{Flatfield}$

Show a before and after Image and also show the Histograms