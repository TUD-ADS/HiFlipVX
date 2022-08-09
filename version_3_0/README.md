## HiFlipVX

Open Source **Hi**gh-Level Synthesis **F**PGA **L**ibrary for **I**mage **P**rocessing

- Image procesing library for FPGAs
- Contains 65 (image, feature and neural network) functions
- 42 funktions based on the OpenVX standard
- 23 functions developed from different algorithms
- Highly optimized, parametrizable and streaming capable
- Most functions support auto-vectorization (parallelization)
- Most functions support additional data-types
- Does not require additional libraries
- Optimized for Xilinx SDSoC / SDAccel / Vivado HLS

## Provided Functions

| **Image Pixelwise**       | **Image Filter**                |
|---------------------------|---------------------------------|
| Absolute Difference       | Box Filter                      |
| Arithmetic Addition       | Conductivity                    |
| Arithmetic Subtraction    | Custom Convolution              |
| Bitwise AND               | Determinant of Hessian          |
| Bitwise EXCLUSIVE OR      | Dilate Image                    |
| Bitwise INCLUSIVE OR      | Erode Image                     |
| Bitwise NOT               | Fast Explicit Diffusion         |
| Data Object Copy          | Gaussian Filter                 |
| Magnitude                 | Hysteresis Filter               |
| Multiply Constant         | Median Filter                   |
| Max                       | Non-Maxima Suppression          |
| Min                       | Oriented Non-Maxima Suppression |
| Phase                     | Scharr 3x3                      |
| Pixel-wise Multiplication | Segment Test Detector           |
| Thresholding              | Sobel                           |
| Weighted Average          |                                 |

| **Image Conversion** | **Image Analysis**        |
|----------------------|---------------------------|
| Channel Combine      | Contrast Factor           |
| Channel Extract      | Equalize Histogram        |
| Color Convert        | Histogram                 |
| Convert Bit Depth    | Integral Image            |
| Convert Data Width   | Mean & Standard Deviation |
| Gather               | Min, Max Location         |
| Multicast            | Scalar Operation          |
| Scale Image          | TableLookup               |
| Scatter              |                           |

| **Feature**         | **Neural Network**     |
|---------------------|------------------------|
| Canny Edge          | 3D-Convolution         |
| Fast Corners        | Activation             |
| Feature Compare     | Batch Normalization    |
| Feature Deserialize | Depthwise Convolution  |
| Feature Extract     | Fully Connected        |
| Feature Gather      | MobileNets Modules 1-3 |
| Feature Multicast   | Pooling                |
| Feature Retain Best | Softmax                |
| ORB Features        |                        |

Windowed operations support different **border handling**:
| Replicated | Pixels beyond borders are replicated            |
| Constant   | Pixels beyond borders are constant zero         |
| Undefined  | Pixels beyond borders do not contain valid data |

Some operations support **conversion policies** against overflow:
| Wrap     | Results are the least significant bits of the output operand. |
| Saturate | Results are saturated to the bit depth of the output operand. |

Some operations support **rounding policies**:
| Round to Zero         | This truncates the least significant values that are lost in operations. |
| Round to Nearest Even | This rounds to nearest even output value.                                |

## Library Files:

The library contains the following files (files with user calable functions and data types are marked in bold letters):

| **File**                     | **Description**                                                                                                                                  |
|------------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------|
| **img_main.cpp**             | Examples of how to   use the implemented _Image_ and _Feature_ functions.                                                                        |
| img_pixelop_test.h           | Functions to test the functionality of   the _Image_ _Pixelwise_ functions.                                                                      |
| **img_pixelop_base.h**       | _Image_ _Pixelwise_ functions to be   called by the user.                                                                                        |
| img_pixelop_core.h           | Sub-functions that are needed for the   _Image_ _Pixelwise_ functions.                                                                           |
| img_filter_test.h            | Functions to test the functionality of   the _Image_ _Filter_ functions.                                                                         |
| **img_filter_base.h**        | _Image_ _Filter_ functions to be called   by the user.                                                                                           |
| img_filter_core.h            | Sub-functions that are needed for the   _Image_ _Filter_ functions.                                                                              |
| img_filter_window.h          | Sub-functions that are needed for the   _Image_ _Filter_ functions.                                                                              |
| img_filter_function.h        | Sub-functions that are needed for the   _Image_ _Filter_ functions.                                                                              |
| img_other_test.h             | Functions to test the functionality of   the _Image_ _Conversion_ & _Analysis_ functions.                                                        |
| **img_conversion_base.h**    | _Image_ _Conversion_ functions to be   called by the user.                                                                                       |
| img_conversion_core.h        | Sub-functions that are needed for the   _Image_ _Conversion_ functions.                                                                          |
| img_mt_conversion_scale.h    | Sub-functions that are needed for the   _Image_ _Conversion_ functions.                                                                          |
| **img_analysis_base.h**      | _Image_ _Analysis_ functions to be called   by the user.                                                                                         |
| img_analysis_core.h          | Sub-functions that are needed for the   _Image_ _Analysis_ functions.                                                                            |
| **img_feature_base.h**       | _Feature_ functions to be called by the   user.                                                                                                  |
| img_feature_core             | Sub-functions that are needed for the   _Feature_ functions.                                                                                     |
| img_mt_feature_test.h        | Functions to test the functionality of   the _Feature_ functions.                                                                                |
| **img_mt_feature_base.h**    | _Feature_ functions to be called by the   user.                                                                                                  |
| img_mt_feature_extract.h     | Sub-functions that are needed for the   _Feature_ functions.                                                                                     |
| img_mt_feature_deserialize.h | Sub-functions that are needed for the   _Feature_ functions.                                                                                     |
| img_mt_feature_compare.h     | Sub-functions that are needed for the   _Feature_ functions.                                                                                     |
| img_mt_feature_gather.h      | Sub-functions that are needed for the   _Feature_ functions.                                                                                     |
| img_mt_helper.h              | Helper functions needed for the _Image_   and _Feature_ functions.                                                                               |
| img_reader.h                 | Functions to read/write pgm images for   testing.                                                                                                |
| **nn_main.cpp**              | Examples of how to use the implemented   _Neural_ _Network_ functions.                                                                           |
| nn_test.h                    | Functions to test the functionality of   the _Neural_ _Network_ functions.                                                                       |
| **nn_mobile_net.h**          | _Neural_ _Network_ functions to be called   by the user.                                                                                         |
| **nn_base.h**                | _Neural_ _Network_ functions to be called   by the user.                                                                                         |
| nn_core1.h                   | Sub-functions that are needed for the   _Neural_ _Network_ functions.                                                                            |
| nn_core2.h                   | Sub-functions that are needed for the   _Neural_ _Network_ functions.                                                                            |
| **vx_helper.h:**             | Contains own data types, enums and   macros. Contains own image data type (*vx_image_data*) and functions to   create and destroy these images.  |
| **vx_types.h:**              | Contains OpenVX data types, enums and   macros (imported from OpenVX).                                                                           |

## Usage
The library files explained in the previous sub-section can be used as follows:
- Add the *..._base.h*, to call functions.
- The *vx_helper.h* contains the needed image data type and functions to create and destroy images
- *vx_types.h* contains the needed OpenVX data types
- The *main.cpp* and *nn_main.cpp* contains examples how to call the implemented functions.
- There are example applications in the main for efficient streaming within one accelerator.

## Needed macros (*vx_helper.h*)
- **__XILINX__** -> Set this flag when using the library in the Xilinx tools
- **__LAST__** -> Creates a LAST signal of the AXI4-stream protocol to *vx_image_data* that indicates the EOF (end of frame) - Set for VDMA & DMA
- **__USER__** -> Creates a USER signal of the AXI4-stream protocol to *vx_image_data* that indicates the SOF (start of frame) - Set for VDMA
- **__URAM__** -> Turns on the usage of XILINX URAM

## Outlook
Further functionality will be added to the library:
- Add AKAZE feature detection function. All ready all sub-functions are inside of the library
- Update the library for Vitis (at the moment LAST and USER signals of the AXI4-stream protocol dont work with Vitis)

## HiFlipVX related Publications
- L. Kalms, A. Podlubne, and D. Göhringer. “HiFlipVX: an Open Source High-Level Synthesis FPGA Library for Image Processing”. In: International Symposium on Applied Reconfigurable Computing (ARC). Springer, Apr. 2019, pp. 149–164. DOI: 10.1007/978-3-030-17227-5_12.
- L. Kalms and D. Göhringer. “Accelerated High-level Synthesis Feature Detection for FPGAs using HiFlipVX”. In: Towards Ubiquitous Low-power Image Processing Platforms. Springer, Jan. 2021, pp. 115–135. DOI: 10.1007/978-3-030-53532-2_7.
- M. A. Davila-Guzman, R. Gran Tejero, M. Villarroya-Gaud, D. Suarez Gracia, L. Kalms, and D. Göhringer. “A Cross-Platform OpenVX Library for FPGA Accelerators”. In: Euromicro International Conference on Parallel, Distributed and Network-based Processing (PDP). IEEE, Mar. 2021, pp. 75–83. DOI: 10.1109/PDP52278.2021.00020.
- L. Kalms, P. Amini Rad, M. Ali, and A. Iskander D. Göhringer. “A Parametrizable High-Level Synthesis Library for Accelerating Neural Networks on FPGAs”. In: Journal of Signal Processing Systems (JSPS). Springer, May 2021, pp. 1–27. DOI: 10.1007/s11265-021-01651-5.
- M. A. Davila-Guzman, L. Kalms, R. Gran Tejero, M. Villarroya-Gaud, D. Suarez Gracia, and D. Göhringer. “A Cross-Platform OpenVX Library for FPGA Accelerators”. In: Journal of Systems Architecture (JSA). Elsevier, Feb. 2022, pp. 1–12. DOI: 10.1016/j.sysarc.2021.102372.
- L. Kalms, T. Häring, and D. Göhringer. “DECISION: Distributing OpenVX Applications on CPUs, GPUs and FPGAs using OpenCL”. In: International Parallel and Distributed Processing Symposium Workshops (IPDPSW). IEEE, May 2022, pp. 1–8. DOI: 10.1109/IPDPSW55747.2022.00023

## Contact Info
M.Sc. Lester Kalms, Technische Universität Dresden, lester.kalms@tu-dresden.de,
M.Sc. Matthias Nickel, Technische Universität Dresden, matthias.nickel@tu-dresden.de,

## Acknowledgment
This work has been partially supported by European Union’s Horizon 2020 research and innovation programme as part of the TULIPP project under grant agreement No 688403 and partially by the German Federal Ministry of Education and Research BMBF as part of the PARIS project under grant agreement number 16ES0657.
