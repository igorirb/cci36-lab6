//
//  hw5.h
//  Lab6
//
//  Created by Igor R. Barroso on 24/10/13.
//  Copyright (c) 2013 Igor Barroso. All rights reserved.
//

#ifndef Lab6_hw5_h
#define Lab6_hw5_h

const int texture_width = 256;
const int texture_height = 256;
GLubyte texture_image[256][256][3];

void generate_texture()
{
    for(int i=0; i<texture_width; i++)
        for(int j=0; j<texture_width; j++)
        {
            texture_image[i][j][0] = 255;
            texture_image[i][j][1] = 255*(i & 0x04);
            texture_image[i][j][2] = 255*(j & 0x04);
        }
}

#endif
