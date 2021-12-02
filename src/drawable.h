#include "includes.h"

class Drawable
{
    public:
        Drawable();
        ~Drawable();
    public:
        virtual void Bind() = 0;
        virtual void Unbind() = 0;
        virtual void Draw() = 0;
};