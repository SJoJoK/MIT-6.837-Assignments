#include <vector>
#include "image.h"
#include "matrix.h"
#include "IFS.h"

// ====================================================================
// ====================================================================
void IFS::ifs_read(const char* input_file)
{
    FILE *input = fopen(input_file, "r");
    assert(input != NULL);
    int num_transforms;
    fscanf(input, "%d", &num_transforms);
    n_trans = num_transforms;
    for (int i = 0; i < num_transforms; i++)
    {
        float probability;
        fscanf(input, "%f", &probability);
        Matrix m;
        m.Read3x3(input);
        probs.push_back(probability);
        trans.push_back(m);
    }
    fclose(input);
    for (int i = 1; i < n_trans; i++)
    {
        probs[i] += probs[i - 1];
    }
    for (int i = 0; i < n_trans; i++)
    {
        trans_cnts.push_back(0);
    }
}
void IFS::ifs_render(Image& img, int n_pts, int n_iters)
{
    img.SetAllPixels(white);
    for (int n = 0; n < n_pts; n++)
    {
        //For lots of random points...
        int x = rand() % img.Width();
        int y = rand() % img.Height();
        Vec2f pos = Vec2f(x, y);
        for (int k = 0; k < n_iters; k++)
        {
            //For numbers of iterations
            float sample = rand() / float(RAND_MAX);
            int i = 0;
            for (i = 0; i < n_trans; i++)
            {
                if (sample > probs[i])
                    continue;
                break;
            }
            //Pick a random transform ans use it
            if (i >= n_trans) i = n_trans - 1;
            Vec2f tmp(pos.x() / (float)img.Width(), pos.y() / img.Height());
            trans[i].Transform(tmp);
            pos.Set(tmp.x() * img.Width(), tmp.y() * img.Height());
            trans_cnts[i]++;
        }
        //display a dot
        if(img.isIn(pos.x(),pos.y()))
            img.SetPixel(pos.x(), pos.y(), black);
    }
    return;
}

// ====================================================================
// ====================================================================
