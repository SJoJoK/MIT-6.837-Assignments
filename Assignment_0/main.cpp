#include "IFS.h"
#include "string.h"
#include <iostream>
int main(int argc, char *argv[])
{
    char *input_file = nullptr;
    int num_points = 10000;
    int num_iters = 10;
    int size = 100;
    char *output_file = nullptr;
    for (int i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-input"))
        {
            i++;
            assert(i < argc);
            input_file = argv[i];
        }
        else if (!strcmp(argv[i], "-points"))
        {
            i++;
            assert(i < argc);
            num_points = atoi(argv[i]);
        }
        else if (!strcmp(argv[i], "-iters"))
        {
            i++;
            assert(i < argc);
            num_iters = atoi(argv[i]);
        }
        else if (!strcmp(argv[i], "-size"))
        {
            i++;
            assert(i < argc);
            size = atoi(argv[i]);
        }
        else if (!strcmp(argv[i], "-output"))
        {
            i++;
            assert(i < argc);
            output_file = argv[i];
        }
        else
        {
            printf("whoops error with command line argument %d: '%s'\n", i, argv[i]);
            assert(0);
        }
    }
    IFS ifs;
    Image img(size, size);
    ifs.ifs_read(input_file);
    ifs.ifs_render(img, num_points, num_iters);
    img.SavePPM(output_file);
    cout << "Times of trans:";
    for (int i = 0; i < ifs.n_trans; i++)
        cout << ifs.trans_cnts[i] << " ";
    cout << endl;
    return 0;
}
