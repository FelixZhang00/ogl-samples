///////////////////////////////////////////////////////////////////////////////////
/// OpenGL Samples Pack (ogl-samples.g-truc.net)
///
/// Copyright (c) 2004 - 2014 G-Truc Creation (www.g-truc.net)
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
/// 
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
/// 
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
///////////////////////////////////////////////////////////////////////////////////

#include "test.hpp"

namespace
{
	std::string const VERT_SHADER_SOURCE("gl-320/texture-compressed.vert");
	std::string const FRAG_SHADER_SOURCE("gl-320/texture-compressed.frag");
	char const * TEXTURE_DIFFUSE_BC1("kueken7_rgb_dxt1_srgb.dds");
	char const * TEXTURE_DIFFUSE_BC3("kueken7_rgba_dxt5_srgb.dds");
	char const * TEXTURE_DIFFUSE_BC4("kueken7_r_ati1n_unorm.dds");
	char const * TEXTURE_DIFFUSE_BC5("kueken7_rg_ati2n_unorm.dds");

	struct vertex
	{
		vertex
		(
			glm::vec2 const & Position,
			glm::vec2 const & Texcoord
		) :
			Position(Position),
			Texcoord(Texcoord)
		{}

		glm::vec2 Position;
		glm::vec2 Texcoord;
	};

	// With DDS textures, v texture coordinate are reversed, from top to bottom
	GLsizei const VertexCount(6);
	GLsizeiptr const VertexSize = VertexCount * sizeof(vertex);
	vertex const VertexData[VertexCount] =
	{
		vertex(glm::vec2(-1.0f,-1.0f), glm::vec2(0.0f, 1.0f)),
		vertex(glm::vec2( 1.0f,-1.0f), glm::vec2(1.0f, 1.0f)),
		vertex(glm::vec2( 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)),
		vertex(glm::vec2( 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)),
		vertex(glm::vec2(-1.0f, 1.0f), glm::vec2(0.0f, 0.0f)),
		vertex(glm::vec2(-1.0f,-1.0f), glm::vec2(0.0f, 1.0f))
	};

	enum texture_type
	{
		TEXTURE_BC1,
		TEXTURE_BC3,
		TEXTURE_BC4,
		TEXTURE_BC5,
		TEXTURE_MAX
	};
	
	namespace shader
	{
		enum type
		{
			VERT,
			FRAG,
			MAX
		};
	}//namespace shader

	std::vector<GLuint> ShaderName(shader::MAX);
	GLuint VertexArrayName(0);
	GLuint ProgramName(0);
	GLuint BufferName(0);
	GLuint TextureName[TEXTURE_MAX] = {0, 0, 0, 0};
	GLint UniformMVP(0);
	GLint UniformDiffuse(0);
	glm::ivec4 Viewport[TEXTURE_MAX] = {glm::ivec4(0), glm::ivec4(0), glm::ivec4(0), glm::ivec4(0)};
}//namespace

class instance : public test
{
public:
	instance(int argc, char* argv[]) :
		test(argc, argv, "gl-320-texture-compressed-ext", test::CORE, 3, 2)
	{}

private:
	bool initProgram()
	{
		bool Validated = true;
	
		if(Validated)
		{
			compiler Compiler;
			GLuint VertShaderName = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE, "--version 150 --profile core");
			GLuint FragShaderName = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE, "--version 150 --profile core");
			Validated = Validated && Compiler.check();

			ProgramName = glCreateProgram();
			glAttachShader(ProgramName, VertShaderName);
			glAttachShader(ProgramName, FragShaderName);

			glBindAttribLocation(ProgramName, semantic::attr::POSITION, "Position");
			glBindAttribLocation(ProgramName, semantic::attr::TEXCOORD, "Texcoord");
			glBindFragDataLocation(ProgramName, semantic::frag::COLOR, "Color");
			glLinkProgram(ProgramName);
			Validated = Validated && Compiler.checkProgram(ProgramName);
		}

		if(Validated)
		{
			UniformMVP = glGetUniformLocation(ProgramName, "MVP");
			UniformDiffuse = glGetUniformLocation(ProgramName, "Diffuse");
		}

		return Validated;
	}

	bool initBuffer()
	{
		glGenBuffers(1, &BufferName);
		glBindBuffer(GL_ARRAY_BUFFER, BufferName);
		glBufferData(GL_ARRAY_BUFFER, VertexSize, VertexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		return true;
	}

	bool initTexture()
	{
		gli::gl GL;

		glActiveTexture(GL_TEXTURE0);
		glGenTextures(TEXTURE_MAX, TextureName);

		{
			gli::texture2D Texture(gli::load(getDataDirectory() + TEXTURE_DIFFUSE_BC1));
			assert(!Texture.empty());

			glBindTexture(GL_TEXTURE_2D, TextureName[TEXTURE_BC1]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, GLint(Texture.levels() - 1));

			gli::gl::format const Format = GL.translate(Texture.format());
			for(std::size_t Level = 0; Level < Texture.levels(); ++Level)
			{
				glCompressedTexImage2D(GL_TEXTURE_2D, GLint(Level),
					Format.Internal,
					GLsizei(Texture[Level].dimensions().x), GLsizei(Texture[Level].dimensions().y),
					0,
					GLsizei(Texture[Level].size()),
					Texture[Level].data());
			}
		}

		{
			gli::texture2D Texture(gli::load(getDataDirectory() + TEXTURE_DIFFUSE_BC3));
			assert(!Texture.empty());

			glBindTexture(GL_TEXTURE_2D, TextureName[TEXTURE_BC3]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, GLint(Texture.levels() - 1));

			gli::gl::format const Format = GL.translate(Texture.format());
			for(std::size_t Level = 0; Level < Texture.levels(); ++Level)
			{
				glCompressedTexImage2D(GL_TEXTURE_2D, GLint(Level),
					Format.Internal,
					static_cast<GLsizei>(Texture[Level].dimensions().x), static_cast<GLsizei>(Texture[Level].dimensions().y), 
					0, 
					static_cast<GLsizei>(Texture[Level].size()),
					Texture[Level].data());
			}
		}

		{
			gli::texture2D Texture(gli::load(getDataDirectory() + TEXTURE_DIFFUSE_BC4));
			assert(!Texture.empty());

			glBindTexture(GL_TEXTURE_2D, TextureName[TEXTURE_BC4]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, GLint(Texture.levels() - 1));

			gli::gl::format const Format = GL.translate(Texture.format());
			for(std::size_t Level = 0; Level < Texture.levels(); ++Level)
			{
				glCompressedTexImage2D(GL_TEXTURE_2D, GLint(Level),
					Format.Internal,
					GLsizei(Texture[Level].dimensions().x), GLsizei(Texture[Level].dimensions().y),
					0,
					GLsizei(Texture[Level].size()),
					Texture[Level].data());
			}
		}

		{
			gli::texture2D Texture(gli::load(getDataDirectory() + TEXTURE_DIFFUSE_BC5));
			assert(!Texture.empty());

			glBindTexture(GL_TEXTURE_2D, TextureName[TEXTURE_BC5]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, GLint(Texture.levels() - 1));

			gli::gl::format const Format = GL.translate(Texture.format());
			for(std::size_t Level = 0; Level < Texture.levels(); ++Level)
			{
				glCompressedTexImage2D(GL_TEXTURE_2D, GLint(Level),
					Format.Internal,
					GLsizei(Texture[Level].dimensions().x), GLsizei(Texture[Level].dimensions().y),
					0,
					GLsizei(Texture[Level].size()),
					Texture[Level].data());
			}
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

		return true;
	}

	bool initVertexArray()
	{
		glGenVertexArrays(1, &VertexArrayName);
		glBindVertexArray(VertexArrayName);
			glBindBuffer(GL_ARRAY_BUFFER, BufferName);
			glVertexAttribPointer(semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), BUFFER_OFFSET(0));
			glVertexAttribPointer(semantic::attr::TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), BUFFER_OFFSET(sizeof(glm::vec2)));
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glEnableVertexAttribArray(semantic::attr::POSITION);
			glEnableVertexAttribArray(semantic::attr::TEXCOORD);
		glBindVertexArray(0);

		return true;
	}

	bool begin()
	{
		glm::ivec2 WindowSize(this->getWindowSize());
		Viewport[TEXTURE_BC1] = glm::ivec4(0, 0, WindowSize >> 1);
		Viewport[TEXTURE_BC3] = glm::ivec4(WindowSize.x >> 1, 0, WindowSize >> 1);
		Viewport[TEXTURE_BC4] = glm::ivec4(WindowSize.x >> 1, WindowSize.y >> 1, WindowSize >> 1);
		Viewport[TEXTURE_BC5] = glm::ivec4(0, WindowSize.y >> 1, WindowSize >> 1);

		bool Validated = true;
		Validated = Validated && this->checkExtension("GL_EXT_texture_compression_s3tc");

		if(Validated)
			Validated = initProgram();
		if(Validated)
			Validated = initBuffer();
		if(Validated)
			Validated = initVertexArray();
		if(Validated)
			Validated = initTexture();

		return Validated;
	}

	bool end()
	{
		glDeleteBuffers(1, &BufferName);
		glDeleteProgram(ProgramName);
		glDeleteTextures(TEXTURE_MAX, TextureName);
		glDeleteVertexArrays(1, &VertexArrayName);

		return true;
	}

	bool render()
	{
		glm::vec2 WindowSize(this->getWindowSize());

		glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, 4.0f / 3.0f, 0.1f, 1000.0f);
		glm::mat4 Model = glm::mat4(1.0f);
		glm::mat4 MVP = Projection * this->view() * Model;

		glClearBufferfv(GL_COLOR, 0, &glm::vec4(1.0f, 0.5f, 0.0f, 1.0f)[0]);

		// Bind the program for use
		glUseProgram(ProgramName);
		glUniformMatrix4fv(UniformMVP, 1, GL_FALSE, &MVP[0][0]);
		glUniform1i(UniformDiffuse, 0);

		glBindVertexArray(VertexArrayName);

		glActiveTexture(GL_TEXTURE0);
		for(std::size_t Index = 0; Index < TEXTURE_MAX; ++Index)
		{
			glViewport(Viewport[Index].x, Viewport[Index].y, Viewport[Index].z, Viewport[Index].w);
			glBindTexture(GL_TEXTURE_2D, TextureName[Index]);
			glDrawArraysInstanced(GL_TRIANGLES, 0, VertexCount, 1);
		}

		return true;
	}
};

int main(int argc, char* argv[])
{
	int Error(0);

	instance Test(argc, argv);
	Error += Test();

	return Error;
}

