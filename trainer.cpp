#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <sstream>

using namespace cv;
using namespace std;

#define NUMPHOTOS 18
#define EPS 0.1
#define MAXBYTE 255.0
#define MAXHASHLENGTH 10001

unsigned int freq_v[MAXHASHLENGTH];

/* std::to_string has a bug
    https://gcc.gnu.org/bugzilla/show_bug.cgi?id=52015
*/
namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm;
        stm << n;
        return stm.str();
    }
}

/* return the first decimal
    ex: 0.54 -> 5
    ex: 0.32 -> 3*/
static int get_first_decimal(float number){
    return static_cast<int>(number*10)%10;
}

/* get pixel by pixel and construct frequency array
    A pixel is mapped in the frequency array with a
    bijective function f(r,g,b) = (r << 8) + (g << 4) + b = pos
*/
void process_image(Mat &image){
    for(unsigned int i = 0; i < image.rows; i++) {
        for(unsigned int j = 0; j < image.cols; j++) {
            Vec3b bgrPixel = image.at<Vec3b>(i, j);
            /* 1 <= r,g,b <= 10 */
            int r = get_first_decimal(bgrPixel.val[2] / MAXBYTE);
            int g = get_first_decimal(bgrPixel.val[1] / MAXBYTE);
            int b = get_first_decimal(bgrPixel.val[0] / MAXBYTE);

            /* all black not allowed here */
            if(!(r == 1 && g == 1 && b == 1))
                freq_v[(r << 8) + (g << 4) + b] ++;
        }
    }
}

/* return the index with the max value */
unsigned int get_max_pixel(){
    
    unsigned int max = 0;
    /* max index */
    unsigned int index = 0;

    for( int i = 1; i < MAXHASHLENGTH; i ++){
        index = max < freq_v[i] ? i : index;
        max = max < freq_v[i] ? freq_v[i] : max;
    }

    return index;
}

int main( int argc, char** argv ){

    Mat image;
    string path(".jpg");

    for(int i = 1; i <= NUMPHOTOS; i ++){
        /* read photo */
        image = imread(("photos/" + patch::to_string(i) + path).c_str(), CV_LOAD_IMAGE_COLOR);

        /* Check for invalid input */
        if(! image.data)
        {
            cout <<  "Could not open or find the image" << std::endl;
            continue;
        }
        process_image(image);
    }

    unsigned int max_ind = get_max_pixel();
    int b = max_ind % 16;
    int g = (max_ind >> 4) % 16;
    int r = (max_ind >> 8) % 16;
    
    printf("r:%d g:%d b:%d\n",r,g,b );
    cout<<"index = "<<max_ind<<" with max = " << freq_v[max_ind]<<endl;         
    
    return 0;
}
