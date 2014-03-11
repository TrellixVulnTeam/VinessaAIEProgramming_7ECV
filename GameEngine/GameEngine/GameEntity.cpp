//By Vinessa

#include "GameEntity.h"


GameEntity::GameEntity(void)
{
}


GameEntity::~GameEntity(void)
{
}
GameEntity::GameEntity( const char* a_pSpriteSheet, GLFWwindow * window)
{
	GameWindow = window;
	elapsedTime = 0;
	LoadVertShader("./Resources/LoadVertexShader.glsl");
	LoadFragShader("./Resources/LoadFragmentShader.glsl");
	LinkShaders();
	GLint uvAttrib = glGetAttribLocation(m_ShaderProgram,"texcoord");
	glEnableVertexAttribArray(uvAttrib);
	matrix_location = glGetUniformLocation (m_ShaderProgram, "matrix");
	LoadSprites(a_pSpriteSheet);
	LoadAnimations(atlas.sAnimations.c_str());
	LoadTexture(atlas.sSheet.c_str());


	m_dFrames = (1.0/15.0);
	currentAnimation = "Idle";
	currentSprite = "Idle_01";
	currentFrame = 0;
	currentPlayType = SINGLE;
	m_uvScale.m_fX = atlas.v2Size.m_fY;
	m_uvScale.m_fY	= atlas.v2Size.m_fX;
	SetSprite();
	SetUVData();
	iFacing = "Right"; //flag for facing


}

void GameEntity::SetSprite()
{
	if(currentAnimation == "")
	{
	m_minUVCoords.m_fX = mSprites["Idle_01"].y0;
	m_minUVCoords.m_fY = mSprites["Idle_01"].x0;
	m_maxUVCoords.m_fX = mSprites["Idle_01"].y1;
	m_maxUVCoords.m_fY = mSprites["Idle_01"].x1;

	m_v2Scale.m_fX =  mSprites["Idle_01"].width;
	m_v2Scale.m_fY =  mSprites["Idle_01"].height;

	}else
	{
		
	m_minUVCoords.m_fX = mSprites[currentSprite].y0;
	m_minUVCoords.m_fY = mSprites[currentSprite].x0;
	m_maxUVCoords.m_fX = mSprites[currentSprite].y1;
	m_maxUVCoords.m_fY = mSprites[currentSprite].x1;
	m_v2Scale.m_fX =  mSprites[currentSprite].width;
	m_v2Scale.m_fY =  mSprites[currentSprite].height;

	}
}


void GameEntity::LoadSprites(const char* a_pSpriteSheet)
{
	
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLNode *rootNode, *currentNode;
	tinyxml2::XMLElement *childElement;
	std::string str;
	doc.LoadFile(a_pSpriteSheet); // load the document
	rootNode = doc.FirstChildElement("atlas");// set the root node
	currentNode = rootNode;

	//currentNode = rootNode->FirstChild(); // set the current node to the root nodes first child
	childElement = currentNode->ToElement();
	atlas.v2Size.m_fX = (float)childElement->IntAttribute("width"); 
	atlas.v2Size.m_fY = (float)childElement->IntAttribute("height");
	atlas.sSheet = childElement->Attribute("sheet");
	atlas.sAnimations = childElement->Attribute("animation");


	for (childElement = currentNode->FirstChildElement(); 
		childElement != NULL; childElement = childElement->NextSiblingElement())
	{
		str = childElement->Attribute("name");
		mSprites[str].Name = str;
		mSprites[str].x0 = childElement->IntAttribute("x");
		mSprites[str].y0 = childElement->IntAttribute("y");
		mSprites[str].height = childElement->IntAttribute("height") ;
		mSprites[str].width = childElement->IntAttribute("width");
		mSprites[str].x1 = mSprites[str].x0 +mSprites[str].width ;
		mSprites[str].y1 = mSprites[str].y0 +mSprites[str].height ;
	

	}
std:printf("done");


}
void GameEntity::LoadAnimations(const char* a_pAnimationSheet)
{
	
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLNode *rootNode, *currentNode;
	tinyxml2::XMLElement *childElement, *child;
	std::string str,aniName;
	doc.LoadFile(a_pAnimationSheet); // load the document
	rootNode = doc.FirstChildElement("animation");// set the root node
	currentNode = rootNode;


	for (childElement = currentNode->ToElement(); 
		childElement != NULL; childElement = childElement->NextSiblingElement())
	{
		aniName = childElement->Attribute("name");
		int i = 0;
		for (child = childElement->FirstChildElement(); 
			child != NULL; child = child->NextSiblingElement())
		{

			str = child->Attribute("name");
			mAnimations[aniName].push_back(str);
			i++;
		}
	}
std:printf("done");

}


void GameEntity::SetAnimation(std::string animation,PlayType type)
{
	currentAnimation = animation;
	currentFrame = 0;
	loopFrame = 0;
	currentPlayType = type;
	switch (type){
	case ONCE:
		
		break;
	case LOOP:
		loopFrame =0;
		break;
	case PINGPONG:
		break;
	case REVERSE:
		currentFrame = mAnimations[currentAnimation].size();
		loopFrame = currentFrame;
		break;
	case RANDOMLOOP:
	case RANDOM:
		srand(time(NULL));
		currentFrame =  (unsigned int)(rand() % mAnimations[currentAnimation].size());
		loopFrame = currentFrame;
		break;
	case LOOPSECTION:
		SetAnimation(animation,type, 0);
	case SINGLE:
		break;
	default:
		break;
	}
	currentSprite = mAnimations.at(currentAnimation)[currentFrame];
	SetSprite();
	SetUVData();

}
void GameEntity::SetAnimation(std::string animation,PlayType type, int frame)
{
	switch(type)
	{
	case LOOPSECTION:
	currentAnimation = animation;
	currentFrame = 0;
	currentPlayType = type;
	loopFrame = frame;
	break;
	default:
		SetAnimation(animation,type);
		break;
	}

}
void GameEntity::PlayAnimation()
{
	elapsedTime += getDeltaTime();
	
	if(elapsedTime > m_dFrames){
		elapsedTime = 0;
	switch (currentPlayType){
	case ONCE:
		if(mAnimations.at(currentAnimation)[currentFrame] != mAnimations.at(currentAnimation).back())
		{
			currentFrame++;
			currentSprite = mAnimations.at(currentAnimation)[currentFrame];
		}
		break;
	case LOOPSECTION:
	case LOOP:
		if(mAnimations.at(currentAnimation)[currentFrame] == mAnimations.at(currentAnimation).back())
		{
			currentFrame = loopFrame;
			currentSprite = mAnimations.at(currentAnimation)[currentFrame];
		}
		else
		{
			currentFrame++;
			currentSprite = mAnimations.at(currentAnimation)[currentFrame];
		}
		break;
	case PINGPONG:
		break;
	case REVERSE:
		currentFrame = mAnimations[currentAnimation].size();
		loopFrame = currentFrame;
		break;
	case RANDOMLOOP:
	case RANDOM:
		srand(time(NULL));
		currentFrame =  rand() % mAnimations[currentAnimation].size();
		loopFrame = currentFrame;
		break;
	case SINGLE:
		break;
	default:
		break;
	}
	SetSprite();
	SetUVData();
	}



}
void GameEntity::Draw()
{


	

	modelMatrix->m_afArray[0]  = m_v2Scale.m_fX *m_fZoom;
	modelMatrix->m_afArray[5]  = m_v2Scale.m_fY *m_fZoom;
	modelMatrix->m_afArray[12] = m_v3Position.m_fX;
	modelMatrix->m_afArray[13] = m_v3Position.m_fY  + ( (m_v2Scale.m_fY *m_fZoom)/2.f);
	modelMatrix->m_afArray[14] = m_v3Position.m_fZ;


	*MVP =  (*Ortho * *modelMatrix) ;


	Quad::Draw();
}

void GameEntity::Input()
{ 
	  if (GLFW_PRESS == glfwGetKey(GameWindow, GLFW_KEY_W))
        {
			SetAnimation("LookUp",ONCE);
			SwapUVs(UP);
			
	  }
	  if 
		  (GLFW_PRESS == glfwGetKey(GameWindow, GLFW_KEY_A))
        {
			SetAnimation("Run",LOOP);
			SwapUVs(LEFT);
                m_v3Position -= Vector3(1.f, 0.0f, 0.0f);
        }

        if (GLFW_PRESS == glfwGetKey(GameWindow, GLFW_KEY_S))
        {
			SetAnimation("Duck",LOOP);
			SwapUVs(DOWN);
			m_v3Position += Vector3(0.0f, -1.f, 0.0f);
		}

		if (GLFW_PRESS == glfwGetKey(GameWindow, GLFW_KEY_D))
			{
				if(FacingCheck("RIGHT") != true) // if the sprite isn't already facing right..
					{
					SetAnimation("TurnAround", ONCE); //Set the animation to turn around...
					SwapUVs(RIGHT); //Swap UV's to flip the sprite...
					iFacing = "Right"; // Sets the facing flag to Right.
						if (currentAnimation !="Run")
						{
							SetAnimation("Run",LOOP);
							m_v3Position += Vector3(1.f, 0.0f, 0.0f);
						}
					}
				else
					SwapUVs(RIGHT);
					 m_v3Position += Vector3(1.f, 0.0f, 0.0f);
        }
	

		 if (GLFW_PRESS == glfwGetKey(GameWindow, GLFW_KEY_V))
        {
			
               SetColor(
				   Vector4((rand()%255)/255.f,(rand()%255)/255.f,(rand()%255)/255.f,1.f),
				   Vector4((rand()%255)/255.f,(rand()%255)/255.f,(rand()%255)/255.f,1.f),
				   Vector4((rand()%255)/255.f,(rand()%255)/255.f,(rand()%255)/255.f,1.f),
				   Vector4((rand()%255)/255.f,(rand()%255)/255.f,(rand()%255)/255.f,1.f)
				   );

        }
}

void GameEntity::Update()
{
	
	
	this->GameEntity::Input();
	this->GameEntity::Draw();
	PlayAnimation();


}
