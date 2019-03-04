#include<iostream>
#include<cmath>
#include<opencv2/opencv.hpp>

using namespace cv;
using namespace std;


uchar interpolate(float x, float y, Mat img);
uchar nearestNeighbor(float x, float y, Mat img);

int main()
{

	/* Prepare matrices for the image, actual coordinates, and new coordinates*/
	string path = "PA1-Images/S1/1.pgm";
	Mat img = imread(path, IMREAD_GRAYSCALE);

	float P_1[] = { 29,55,1,  63,55,1,  48,73,1,  48,89,1 };
	float hatX[] = {10, 28, 20, 20};
	float hatY[] = {11, 11, 30, 42};

	//Size(y,x) where x = num of rows and y = num of cols desired in the matrix.
	Mat A(Size(3, 4), CV_32FC1, P_1);
	Mat bX(Size(1, 4), CV_32FC1, hatX);
	Mat bY(Size(1, 4), CV_32FC1, hatY);

	/* Prepare matrices U, D, and Vt to store SVD, then compute the SVD*/
	Mat D, U, Vt;
	SVD::compute(A, D, U, Vt);
	
/*	cout << "Original Matrix:\n" << A << endl << endl
		 << "Left Singular Values Matrix:\n" << U << endl << endl
 		 << "Singular Values:\n" << D << endl << endl
		 << "Right Singular Values Matrix (Transposed):\n" << Vt << endl << endl;
*/
/****************************************************************************
* Find our affine transformation matrix 'x' containing a_11, a_12, b_1, etc.
* where matrix 'b' is either the predetermined matrix 'hatX' or 'hatY'.
* Recall that  1) Ax=b , x=(A^-1)b )
*         and  2) A^-1 = (V^T)^T (D^-1) (U^T)
*         so   3) [(V^T)^T (D^-1) (U^T)](b) = x
* Matrix 'x' can be solved for with SVD::backSubst( rhs, dst),
* where rhs = right hand side of eqn(1)Ax=b, 'b' with A in SVD form,
* in this case bX and bY.
*       lhs = left hand side of eqn(1), in this case 'x', the solution.
****************************************************************************/
	// D is 1x3 matrix, but should be 3x3 where elements of D are the diagnol and everything else is 0.
	//Actually, AVD::backSubst still works without turing D into a diagnol matrix.

	// c_1 holds a_11, a_12, and b_1 for finding X	
	Mat c_1; //To store the solution
	SVD::backSubst(D, U, Vt, bX, c_1);

	// c_2 holds a_21, a_22, and b_2 for finding Y
	Mat c_2; //To store the solution
	SVD::backSubst(D, U, Vt, bY, c_2);

	cout << "c_1:\n" << c_1 << endl << endl
		 << "c_2:\n" << c_2 << endl << endl;

	/****************************************************************************
	* Now that we have c_1 and c_2, we can convert it to square matrix as a
	* homogenous system, then take the inverse to grab pixel coordinates from the
	* original using Ax = b where A is the matrix made of c_1' and c_2',
	* x is the matrix of the original, and b is the coordinates in the output.
	****************************************************************************/

	// ' means transpose
	//turn c_1' and c_2' into a homogenous matrix
	float bigC[] = { c_1.at<float>(0),c_1.at<float>(1),c_1.at<float>(2),
					 c_2.at<float>(0),c_2.at<float>(1),c_2.at<float>(2),
					 0,0, 1 };
	Mat Ab(Size(3,3), CV_32FC1, bigC);
	Mat Abinv = Ab.inv();
	
	cout << "Ab:\n" << Ab << endl << endl
		 << "Ab^-1:\n" << Abinv << endl << endl;

	/************************************************************************
	* I think we're ready to do a for loop here to go through the output
	* 48x40 image matrix to find the corresponding color value in the original 
	* image.
	*************************************************************************/
	Mat outputa(Size(40,48), CV_8U);// Using Interpolation
	Mat outputb(Size(40,48), CV_8U);// Using Nearest Neighbor

	for(float i=0; i<48; i++)
	{
		for(float j=0; j<40; j++)
		{
			float coord[] = {i, j, 1};
			Mat output(Size(1,3), CV_32FC1, coord);	
			Mat coriginal = Abinv*output;
			Mat original = coriginal;// / coriginal.at<float>(2));
			
			if( ((i==10) && (j==11)) ||
				((i==28) && (j==11)) ||
				((i==20) && (j==30)) ||
				((i==20) && (j==42))    )
			{
				cout << "(" << i << "," << j << "): \n"
					 << "coriginal: " << coriginal.t() << endl
					 << "original: " << original.t() << endl << endl;
			}
			
			outputa.at<uchar>(i,j) = interpolate(original.at<float>(0), 
												 original.at<float>(1), img);
			outputb.at<uchar>(i,j) = nearestNeighbor(original.at<float>(0),
													 original.at<float>(1), img);

			namedWindow("inter",WINDOW_AUTOSIZE);
			namedWindow("n_n",WINDOW_AUTOSIZE);

			imshow("inter", outputa);
			imshow("n_n", outputb);
			
			waitKey(1);

		}
	}


	/***************************************************************************
	*                         Show results and original                        *
	****************************************************************************/
	namedWindow("original",WINDOW_AUTOSIZE); // creating named windows for display
	imshow("original", img); // showing matrices as images in windows
	waitKey(0); // keep image windows from closing until key press

	return 0;
}

uchar interpolate(float x, float y, Mat img)
{
	int flrX = floor(x);
	int flrY = floor(y);
	int ceilX = ceil(x);
	int ceilY = ceil(y);
	
	uchar value1 = img.at<uchar>(flrX, flrY);
	uchar value2 = img.at<uchar>(flrX, ceilY);
	uchar value3 = img.at<uchar>(ceilX, flrY);
	uchar value4 = img.at<uchar>(ceilX, ceilY);
	
	return (value1 + value2 + value3 + value4)/4;
}

uchar nearestNeighbor(float x, float y, Mat img)
{

	int rd_x = round(x);
	int rd_y = round(y);

	return img.at<uchar>(rd_x,rd_y);
}

