#include "debug.hpp"

namespace fp
{

    void printGLContext(GLContext *c)
    {
        if (c == nullptr)
        {
            std::cout << "GLContext is null." << std::endl;
            return;
        }

        std::cout << "===== GLContext Contents =====" << std::endl;

        // Print Z-buffer information
        std::cout << "ZBuffer:" << std::endl;
        if (c->zb != nullptr)
        {
            std::cout << "  Width: " << c->zb->xsize << std::endl;
            std::cout << "  Height: " << c->zb->ysize << std::endl;
            std::cout << "  Mode: " << c->zb->mode << std::endl;
        }
        else
        {
            std::cout << "  ZBuffer is null." << std::endl;
        }

        // Print viewport information
        std::cout << "Viewport:" << std::endl;
        std::cout << "  xmin: " << c->viewport.xmin << std::endl;
        std::cout << "  ymin: " << c->viewport.ymin << std::endl;
        std::cout << "  xsize: " << c->viewport.xsize << std::endl;
        std::cout << "  ysize: " << c->viewport.ysize << std::endl;
        std::cout << "  Scale: (" << c->viewport.scale.X << ", " << c->viewport.scale.Y << ", " << c->viewport.scale.Z << ")" << std::endl;
        std::cout << "  Trans: (" << c->viewport.trans.X << ", " << c->viewport.trans.Y << ", " << c->viewport.trans.Z << ")" << std::endl;

        // Print current texture information
        std::cout << "Texture State:" << std::endl;
        std::cout << "  Enabled 2D: " << c->texture.enabled_2d << std::endl;
        std::cout << "  Env Mode: " << c->texture.env_mode << std::endl;

        if (c->texture.current != nullptr)
        {
            GLTexture *tex = c->texture.current;
            std::cout << "  Current Texture Handle: " << tex->handle << std::endl;
            std::cout << "  Wrap S: " << tex->wrap_s << std::endl;
            std::cout << "  Wrap T: " << tex->wrap_t << std::endl;
            std::cout << "  Min Filter: " << tex->min_filter << std::endl;
            std::cout << "  Mag Filter: " << tex->mag_filter << std::endl;
            std::cout << "  Is Complete: " << tex->is_complete << std::endl;

            // Print information about level 0 image
            GLImage &image = tex->images[0];
            std::cout << "  Level 0 Image:" << std::endl;
            std::cout << "    Width: " << image.xsize << std::endl;
            std::cout << "    Height: " << image.ysize << std::endl;
            std::cout << "    Internal Format: " << image.internal_format << std::endl;
            std::cout << "    Format: " << image.format << std::endl;
            std::cout << "    Type: " << image.type << std::endl;
            std::cout << "    Pixmap: " << (void *)image.pixmap << std::endl;
        }
        else
        {
            std::cout << "  No texture currently bound." << std::endl;
        }

        // Print error code
        std::cout << "Error Code: " << c->error << std::endl;

        // Print pixel storage modes
        std::cout << "Pixel Storage Modes:" << std::endl;
        std::cout << "  Unpack Alignment: " << c->unpack_alignment << std::endl;

        // Print current color
        std::cout << "Current Color:" << std::endl;
        std::cout << "  (" << c->current.color.X << ", " << c->current.color.Y << ", "
                  << c->current.color.Z << ", " << c->current.color.W << ")" << std::endl;

        // Print other relevant states as needed
        // ...

        std::cout << "===== End of GLContext Contents =====" << std::endl;
    }

} // namespace fp
