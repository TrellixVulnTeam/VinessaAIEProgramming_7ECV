
//by vinessa
#include "Sprite.h"

Sprite::Sprite(void)
{

	const char * VertexShader = // Vertex Shaders deal with objects in 3D Space
		"#version 330\n"
		"layout(location = 0) in vec3 position;" //what do these even do? why is it 1 and 0?
		"layout(location = 1) in vec4 color;"
		"out vec4 vColor;"
		"void main() {"
		"vColor = color;"
		"gl_Position = vec4 (position, 1.0);"
		"}";

	const char * FragmentShader = // Fragment Shaders deal with pixel data... inbetween vertexes?
		"#version 330\n"
		"in vec4 vColor;"
		"void main() {"
		"  vColor = color;"
		"  gl_Position = vec4 (position, 1.0);"
		"}";


	//compiling the vertex shader!
	m_VertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(m_VertexShader, 1, &VertexShader, NULL);
	glCompileShader(m_VertexShader);
	printShaderInfoLog(m_VertexShader);

	//compile the fragment shader!
	m_FragmentShader = glCreateShader (GL_FRAGMENT_SHADER);
	glShaderSource(m_VertexShader, 1, &FragmentShader, NULL);
	glCompileShader(m_FragmentShader);
	printShaderInfoLog(m_FragmentShader);

	//Link Shaders into the Shader program

	m_ShaderProgram = glCreateProgram();

	glAttachShader(m_ShaderProgram, m_FragmentShader);
	glAttachShader(m_ShaderProgram, m_VertexShader);

	glLinkProgram(m_ShaderProgram);
	printProgramInfoLog(m_ShaderProgram);
	glUseProgram(m_ShaderProgram);

	AnimationFrameRun = 0;
	AnimationFrameJump = 0;

}

Sprite::~Sprite(void)
{
}

Sprite::Sprite(const char* a_pTexture, int a_iWidth, int a_iHeight, tbyte::Vector4 a_v4Color,GLFWwindow* window, float a_AniOffsetU,float a_AniOffsetV, float Z_Position,tbyte::Vector3 UVCoord1,tbyte::Vector3 UVCoord2,tbyte::Vector3 UVCoord3,tbyte::Vector3 UVCoord4)
{
	float Framespeed = 0.00004;
	PlaySpeedTimer = Vi_Timer(Framespeed);
	PlaySpeedTimer.SetStartTime();
	GameWindow = window;
	AnimationFrameRun = 0;
	AnimationFrameJump = 0;
	AnimationOffsetU = a_AniOffsetU;
	AnimationOffsetV = a_AniOffsetV;

	m_v2Scale = tbyte::Vector2(a_iWidth, a_iHeight);
	m_v3Position = tbyte::Vector3(m_v2Scale.m_fX,m_v2Scale.m_fY,.5);

	modelMatrix = new tbyte::Matrix4();
	*modelMatrix = modelMatrix->MakeIdentityMatrix();
	modelMatrix->m_afArray[12] = m_v3Position.m_fX;
	modelMatrix->m_afArray[13] = m_v3Position.m_fY;
	modelMatrix->m_afArray[14] = m_v3Position.m_fZ;


	//proj_location = glGetUniformLocation(m_ShaderProgram, "projection");

	LoadVertexShader("./Resources/LoadVertexShader.glsl");
	LoadFragmentShader("./Resources/LoadFragmentShader.glsl");
	LinkShaders();

	//////////////////////////


	m_v4SpriteColor = a_v4Color;
	m_aoVerts[0].Pos = tbyte::Vector3(-0.5f, 0.5f, Z_Position);
	m_aoVerts[1].Pos = tbyte::Vector3(0.5f, 0.5f, Z_Position);
	m_aoVerts[2].Pos = tbyte::Vector3(-0.5f, -0.5f, Z_Position);
	m_aoVerts[3].Pos = tbyte::Vector3(0.5f, -0.5f, Z_Position);

	m_aoVerts[0].Color = m_v4SpriteColor;
	m_aoVerts[1].Color = m_v4SpriteColor;
	m_aoVerts[2].Color = m_v4SpriteColor;
	m_aoVerts[3].Color = m_v4SpriteColor;

	m_aoVerts[0].UV = tbyte::Vector2(0.0f, 0.0f);
	m_aoVerts[1].UV = tbyte::Vector2(0.0f, 0.0835f);
	m_aoVerts[2].UV = tbyte::Vector2(0.167f, 0.0f);
	m_aoVerts[3].UV = tbyte::Vector2(0.167f, 0.0835f);

	GLuint elements[] =
	{
		0,1,2,3
	};



	//Generate the BUFFARS!

	glGenBuffers(1, &m_VertexBuffer);
	glGenBuffers(1, &m_ElementBuffer);
	glGenVertexArrays(1, &m_VertexAttribute);

	// BIND THE BUFFARS

	glBindVertexArray (m_VertexAttribute);
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ElementBuffer);

	//Putting teh datas into teh buffer

	glBufferData(GL_ARRAY_BUFFER, 4* sizeof(Vertex), m_aoVerts, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	//enabling attributes

	GLint posAttrib = glGetAttribLocation (m_ShaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	GLint colAttrib = glGetAttribLocation (m_ShaderProgram, "color");
	glEnableVertexAttribArray(colAttrib);
	GLint uvAttrib = glGetAttribLocation (m_ShaderProgram, "texcoord");
	glEnableVertexAttribArray(uvAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(colAttrib,4, GL_FLOAT, GL_FALSE, sizeof(Vertex),(void*) (3 * sizeof(float)));
	glVertexAttribPointer(uvAttrib,2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(7 * sizeof(float)));
	glBindVertexArray(0);



	matrix_location = glGetUniformLocation (m_ShaderProgram, "matrix");

	m_uiTexture = 0;
	m_minUVCoords = tbyte::Vector2(0.f, 0.f);
	m_maxUVCoords = tbyte::Vector2(1.f,1.f);
	m_uvScale = tbyte::Vector2( 1.f, 1.f);
	m_uvOffset = tbyte::Vector2( 0.f, 0.f);

	m_uSourceBlendMode = GL_SRC_ALPHA;
	m_uDestinationBlendMode = GL_ONE_MINUS_SRC_ALPHA;
	glGenTextures(1, &m_uiTexture);
	glActiveTexture (GL_TEXTURE0);

	int width, height;
	unsigned char* image = SOIL_load_image (a_pTexture, &width, &height, 0, SOIL_LOAD_RGBA);
	glBindTexture( GL_TEXTURE_2D, m_uiTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	tex_loc = glGetUniformLocation (m_ShaderProgram, "diffuseTexture");
	glUseProgram (m_ShaderProgram);
	glUniform1i (tex_loc, 0); //use active texture 0
	MVP = MVP.MakeIdentityMatrix();
}

void Sprite::Draw()
{
	glBlendFunc (m_uSourceBlendMode, m_uDestinationBlendMode);
	glUseProgram(m_ShaderProgram);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture (GL_TEXTURE_2D, m_uiTexture);
	glUniform1i (tex_loc, 0);
	modelMatrix->m_afArray[0] = m_v2Scale.m_fX;
	modelMatrix->m_afArray[5] = m_v2Scale.m_fY;
	modelMatrix->m_afArray[12] = m_v3Position.m_fX;
	modelMatrix->m_afArray[13] = m_v3Position.m_fY;
	modelMatrix->m_afArray[14] = m_v3Position.m_fZ;

	MVP = (*Ortho * *modelMatrix);
	glUniformMatrix4fv (matrix_location, 1, GL_FALSE, MVP.m_afArray);
	//justin added for animation
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, 4* sizeof(Vertex), m_aoVerts, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ElementBuffer);
	glBindVertexArray(m_VertexAttribute);
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, 0);


}



void Sprite::Input()
{
	if (GLFW_PRESS == glfwGetKey(GameWindow, GLFW_KEY_W))
	{
		//m_v3Position += tbyte::Vector3(0.0f, 0.005f, 0.0f);
		AnimateRunR();
	}

	if (GLFW_PRESS == glfwGetKey (GameWindow, GLFW_KEY_A))
	{
		//m_v3Position += tbyte::Vector3((-0.05f*PlaySpeedTimer.DeltaTime), 0.0f, 0.0f);
		AnimateRunL();
	}

	if (GLFW_PRESS == glfwGetKey (GameWindow, GLFW_KEY_S))
	{
		//m_v3Position += tbyte::Vector3(0.0f, (-0.05f*PlaySpeedTimer.DeltaTime), 0.0f);
		AnimateRunL();
	}

	if (GLFW_PRESS == glfwGetKey (GameWindow, GLFW_KEY_D))
	{
		//m_v3Position += tbyte::Vector3((0.05f*PlaySpeedTimer.DeltaTime), 0.0f, 0.0f);
		AnimateRunR();
	}

	if (GLFW_PRESS == glfwGetKey (GameWindow, GLFW_KEY_SPACE))
	{
		//m_v3Position += tbyte::Vector3((0.05f*PlaySpeedTimer.DeltaTime), 0.0f, 0.0f);
		AnimateJumpRight();
	}

	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//Justin Debug Only!!!!!!!!!!!!!!!!!
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	if (GLFW_PRESS == glfwGetKey(GameWindow, GLFW_KEY_C))
		m_v3Position = tbyte::Vector3(m_v2Scale.m_fX+1, m_v2Scale.m_fY+1, 0.0f);
}

inline void Sprite::SetVertexData(Vertex* a_vertexData)
{
	memcpy(&m_aoVerts, a_vertexData, sizeof(Vertex) *4);
}

inline const Vertex* Sprite::GetVertexData() const
{
	return static_cast<const Vertex*>(m_aoVerts);
}



void Sprite::AnimateRunR()
{ 
	AnimationFrameJump = 0;

	if((PlaySpeedTimer.TimeUpCheck()) == true)
		if (AnimationFrameRun == 0)
		{
			AnimationFrameRun++;
			m_aoVerts[0].UV = tbyte::Vector2(0.0f, 0.0f);
			m_aoVerts[1].UV = tbyte::Vector2(0.0f, 0.0835f);
			m_aoVerts[2].UV = tbyte::Vector2(0.167f, 0.0f);
			m_aoVerts[3].UV = tbyte::Vector2(0.167f, 0.0835f);
			PlaySpeedTimer.SetStartTime();
		}

		else
			if((AnimationFrameRun < 12)&&(AnimationFrameRun > 0))
			{
				AnimationFrameRun++;
				m_aoVerts[0].U +=  AnimationOffsetU;
				m_aoVerts[1].U +=  AnimationOffsetU;
				m_aoVerts[2].U +=  AnimationOffsetU;
				m_aoVerts[3].U +=  AnimationOffsetU;
				PlaySpeedTimer.SetStartTime();


			}

			else
				if(AnimationFrameRun >= 12)
				{
					AnimationFrameRun = 0;

					m_aoVerts[0].UV = tbyte::Vector2(0.0f, 0.0f);
					m_aoVerts[1].UV = tbyte::Vector2(0.0f, 0.0835f);
					m_aoVerts[2].UV = tbyte::Vector2(0.167f, 0.0f);
					m_aoVerts[3].UV = tbyte::Vector2(0.167f, 0.0835f);
					PlaySpeedTimer.SetStartTime();
				}



}

void Sprite::AnimateRunL()
{	
	AnimationFrameJump = 0;

	if((PlaySpeedTimer.TimeUpCheck()) == true)
		if (AnimationFrameRun == 0)
		{
			AnimationFrameRun++;
			m_aoVerts[0].UV = tbyte::Vector2(0.167f, 0.0f);
			m_aoVerts[1].UV = tbyte::Vector2(0.167f, 0.0835f);
			m_aoVerts[2].UV = tbyte::Vector2(0.334f, 0.0f);
			m_aoVerts[3].UV = tbyte::Vector2(0.334f, 0.0835f);
			PlaySpeedTimer.SetStartTime();
		}
		else
			if((AnimationFrameRun < 12)&&(AnimationFrameRun > 0))
			{
				AnimationFrameRun++;

				m_aoVerts[0].U +=  AnimationOffsetU;
				m_aoVerts[1].U +=  AnimationOffsetU;
				m_aoVerts[2].U +=  AnimationOffsetU;
				m_aoVerts[3].U +=  AnimationOffsetU;
				PlaySpeedTimer.SetStartTime();


			}

			else
				if(AnimationFrameRun >= 12)
				{
					AnimationFrameRun = 0;

					m_aoVerts[0].UV = tbyte::Vector2(0.167f, 0.0f);
					m_aoVerts[1].UV = tbyte::Vector2(0.167f, 0.0835f);
					m_aoVerts[2].UV = tbyte::Vector2(0.334f, 0.0f);
					m_aoVerts[3].UV = tbyte::Vector2(0.334f, 0.0835f);
					PlaySpeedTimer.SetStartTime();
				}



}

void Sprite::AnimateJumpRight()
{ 
	AnimationFrameRun = 0;
	if((PlaySpeedTimer.TimeUpCheck()) == true)
		if (AnimationFrameJump == 0)
		{
			AnimationFrameJump++;
			m_aoVerts[0].V = 0.334f;
			m_aoVerts[1].V = 0.334f;
			m_aoVerts[2].V = 0.501f;
			m_aoVerts[3].V = 0.501f;

			m_aoVerts[0].U =  0.0f;
			m_aoVerts[1].U =  AnimationOffsetU;
			m_aoVerts[2].U =  0.0f;
			m_aoVerts[3].U =  AnimationOffsetU;
			PlaySpeedTimer.SetStartTime();

		}
		else
			if((AnimationFrameJump < 12)&&(AnimationFrameJump > 0))
			{
				AnimationFrameJump++;

				/*m_aoVerts[0].V = 0.501f;
				m_aoVerts[1].V = 0.501f;
				m_aoVerts[2].V = 0.668f;
				m_aoVerts[3].V = 0.668f;*/

				m_aoVerts[0].U +=  AnimationOffsetU;
				m_aoVerts[1].U +=  AnimationOffsetU;
				m_aoVerts[2].U +=  AnimationOffsetU;
				m_aoVerts[3].U +=  AnimationOffsetU;
				PlaySpeedTimer.SetStartTime();


			}

			else
				if((AnimationFrameJump >= 12) && (AnimationFrameJump < 23))
				{
					AnimationFrameJump++;

					m_aoVerts[0].V = 0.501f;
					m_aoVerts[1].V = 0.501f;
					m_aoVerts[2].V = 0.668f;
					m_aoVerts[3].V = 0.668f;

					m_aoVerts[0].U -=  AnimationOffsetU;
					m_aoVerts[1].U -=  AnimationOffsetU;
					m_aoVerts[2].U -=  AnimationOffsetU;
					m_aoVerts[3].U -=  AnimationOffsetU;
					PlaySpeedTimer.SetStartTime();
				}

				else
					if (AnimationFrameJump >= 23)
					{
						AnimationFrameJump = 0;

						m_aoVerts[0].V = 0.501f;
						m_aoVerts[1].V = 0.501f;
						m_aoVerts[2].V = 0.668f;
						m_aoVerts[3].V = 0.668f;

						m_aoVerts[0].U =  0.0f;
						m_aoVerts[1].U =  AnimationOffsetU;
						m_aoVerts[2].U =  0.0f;
						m_aoVerts[3].U =  AnimationOffsetU;
						PlaySpeedTimer.SetStartTime();
					}



}

void Sprite::AnimateJumpLeft()
{ 
	AnimationFrameRun = 0;
	if((PlaySpeedTimer.TimeUpCheck()) == true)
		if (AnimationFrameJump == 0)
		{
			AnimationFrameJump++;
			m_aoVerts[0].V = 0.668f;
			m_aoVerts[1].V = 0.668f;
			m_aoVerts[2].V = 0.835f;
			m_aoVerts[3].V = 0.835f;

			m_aoVerts[0].U =  0.0f;
			m_aoVerts[1].U =  AnimationOffsetU;
			m_aoVerts[2].U =  0.0f;
			m_aoVerts[3].U =  AnimationOffsetU;
			PlaySpeedTimer.SetStartTime();

		}
		else
			if((AnimationFrameJump < 12)&&(AnimationFrameJump > 0))
			{
				AnimationFrameJump++;

				/*	m_aoVerts[0].V = 0.501f;
				m_aoVerts[1].V = 0.501f;
				m_aoVerts[2].V = 0.668f;
				m_aoVerts[3].V = 0.668f;*/

				m_aoVerts[0].U +=  AnimationOffsetU;
				m_aoVerts[1].U +=  AnimationOffsetU;
				m_aoVerts[2].U +=  AnimationOffsetU;
				m_aoVerts[3].U +=  AnimationOffsetU;
				PlaySpeedTimer.SetStartTime();


			}

			else
				if((AnimationFrameJump >= 12) && (AnimationFrameJump < 23))
				{
					AnimationFrameJump++;

					m_aoVerts[0].V = 0.835f;
					m_aoVerts[1].V = 0.835f;
					m_aoVerts[2].V = 1.0f;
					m_aoVerts[3].V = 1.0f;

					m_aoVerts[0].U -=  AnimationOffsetU;
					m_aoVerts[1].U -=  AnimationOffsetU;
					m_aoVerts[2].U -=  AnimationOffsetU;
					m_aoVerts[3].U -=  AnimationOffsetU;
					PlaySpeedTimer.SetStartTime();
				}

				else
					if (AnimationFrameJump >= 23)
					{
						AnimationFrameJump = 0;

						m_aoVerts[0].V = 0.668f;
						m_aoVerts[1].V = 0.668f;
						m_aoVerts[2].V = 0.835f;
						m_aoVerts[3].V = 0.835f;

						m_aoVerts[0].U =  0.0f;
						m_aoVerts[1].U =  AnimationOffsetU;
						m_aoVerts[2].U =  0.0f;
						m_aoVerts[3].U =  AnimationOffsetU;
						PlaySpeedTimer.SetStartTime();
					}



}