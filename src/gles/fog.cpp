#include "zgl.hpp"

void glFogx(GLenum pname, tGLfixed param)
{
	switch(pname)
	{
		case GL_FOG_MODE:
			switch((GLint)param)
			{
				case GL_LINEAR:
					break;
				case GL_EXP:
					break;
				case GL_EXP2:
					break;
				default:
                    break;
			}
			break;

		case GL_FOG_DENSITY:
			break;

		case GL_FOG_START:
			break;

		case GL_FOG_END:
			break;

        // TODO:
//		case GL_FOG_INDEX:
//			break;
			
		case GL_FOG_COLOR:
			break;

		default:
			break;
	}
}

void glFogxv(GLenum pname, tGLfixed *param)
{
	switch(pname)
	{
		case GL_FOG_MODE:
			switch((GLint)*param)
			{
				case GL_LINEAR:
					break;
				case GL_EXP:
					break;
				case GL_EXP2:
					break;
				default:
                    break;
			}
			break;
			
		case GL_FOG_DENSITY:
			break;
			
		case GL_FOG_START:
			break;
			
		case GL_FOG_END:
			break;
			
//		case GL_FOG_INDEX:
//			break;
			
		case GL_FOG_COLOR:
			break;
			
		default:
			break;
	}
}
