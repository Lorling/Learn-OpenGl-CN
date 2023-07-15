#include "Game.h"
#include "ResourceManager.h"
#include <iostream>
#include <GLFW/glfw3.h>

//��ʼ����ɫ�����С
const glm::vec2 PLAYER_SIZE(100, 20);
//��ʼ����ɫ�����ٶ�
const GLfloat PLAYER_VELOCITY(500.0f);
//��ʼ������ٶ�
const glm::vec2 BALL_VELOCITY(100.0f, -350.f);
//��İ뾶
const GLfloat BALL_RADIUS = 12.5f;

SpriteRenderer* Renderer;
GameObject* Player;
BallObject* Ball;
ParticleGenerator* Particles;
PostProcessor* Effects;

GLfloat ShakeTime = 0.0f;

Game::Game(GLfloat width, GLfloat height) :
	State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{
}

Game::~Game()
{
	delete Renderer;
	delete Player;
	delete Ball;
	delete Particles;
	delete Effects;
}

void Game::Init()
{
	//������ɫ��
	ResourceManager::LoadShader("Breakout/shaders/Sprite.shader", "sprite");
	ResourceManager::LoadShader("Breakout/shaders/Particle.shader", "particle");
	ResourceManager::LoadShader("Breakout/shaders/Scene.shader", "scene");

	glm::mat4 projection = glm::ortho(0.0f, Width, Height, 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").Use();
	ResourceManager::GetShader("sprite").SetUniformInt("image", 0);
	ResourceManager::GetShader("sprite").SetUniformMatrix4fv("projection", projection);
	ResourceManager::GetShader("particle").Use();
	ResourceManager::GetShader("particle").SetUniformInt("image", 0);
	ResourceManager::GetShader("particle").SetUniformMatrix4fv("projection", projection);

	//��������
	ResourceManager::LoadTexture("res/textures/background.jpg", "background");
	ResourceManager::LoadTexture("res/textures/block.png", "block");
	ResourceManager::LoadTexture("res/textures/block_solid.png", "block_solid");
	ResourceManager::LoadTexture("res/textures/paddle.png", "paddle");
	ResourceManager::LoadTexture("res/textures/awesomeface.png", "ball");
	ResourceManager::LoadTexture("res/textures/particle.png", "particle");
	ResourceManager::LoadTexture("res/textures/powerup_chaos.png", "chaos");
	ResourceManager::LoadTexture("res/textures/powerup_confuse.png", "confuse");
	ResourceManager::LoadTexture("res/textures/powerup_increase.png", "increase");
	ResourceManager::LoadTexture("res/textures/powerup_passthrough.png", "passthrough");
	ResourceManager::LoadTexture("res/textures/powerup_sticky.png", "sticky");
	ResourceManager::LoadTexture("res/textures/powerup_speed.png", "speed");
	//���عؿ�
	GameLevel one, two, three, four;
	one.Load("res/levels/one.txt", Width, Height * 0.5f);
	two.Load("res/levels/one.txt", Width, Height * 0.5f);
	three.Load("res/levels/one.txt", Width, Height * 0.5f);
	four.Load("res/levels/one.txt", Width, Height * 0.5f);
	levels.push_back(one);
	levels.push_back(two);
	levels.push_back(three);
	levels.push_back(four);
	level = 0;
	//��ʼ����ɫ����
	glm::vec2 playerPos = glm::vec2(Width / 2 - PLAYER_SIZE.x / 2, Height - PLAYER_SIZE.y);
	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));
	//��ʼ����
	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -2 * BALL_RADIUS);//�����Ͻǵ�λ��
	Ball = new BallObject(ballPos, BALL_RADIUS, BALL_VELOCITY, ResourceManager::GetTexture("ball"));

	Particles = new ParticleGenerator(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("particle"), 500);

	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));

	Effects = new PostProcessor(ResourceManager::GetShader("scene"), Width, Height);
}

void Game::ProccessInput(GLfloat dt)
{
	if (State == GAME_ACTIVE) {
		GLfloat velocity = PLAYER_VELOCITY * dt;
		//�ƶ����壬����������ŵ����ƶ�
		if (Keys[GLFW_KEY_A]) {
			if (Player->Position.x >= 0) {
				Player->Position.x -= velocity;
				if (Ball->Stuck) {
					if (Ball->Position.x >= 0)
						Ball->Position.x -= velocity;
				}
			}
			if (Player->Position.x < 0) Player->Position.x = 0;
		}
		if (Keys[GLFW_KEY_D]) {
			if (Player->Position.x <= Width - Player->Size.x) {
				Player->Position.x += velocity;
				if (Ball->Stuck)
					if (Ball->Position.x <= Width - Ball->Radius * 2) {
						Ball->Position.x += velocity;
					}
			}
			if (Player->Position.x > Width - Player->Size.x) Player->Position.x = Width - Player->Size.x;
		}
		//�������ո����ͷ���
		if (Keys[GLFW_KEY_SPACE]) {
			Ball->Stuck = GL_FALSE;
		}
	}
}

void Game::Update(GLfloat dt)
{
	//�������λ��
	Ball->Move(dt, Width);
	//�����ײ
	DoCollisions();
	//�������ײ�ײ��߽�����
	if (Ball->Position.y >= Height) {
		ResetLevel();
		ResetPlayer();
		//�������
		PowerUps.clear();
		Effects->Confuse = GL_FALSE;
		Effects->Chaos = GL_FALSE;
	}
	//���Ӹ��£����Ҳ���������ƫ��������
	Particles->Update(dt, *Ball, 2, glm::vec2(Ball->Radius / 2));
	//����shaketime
	if (ShakeTime > 0.0f) {
		ShakeTime -= dt;
		if (ShakeTime <= 0.0f)
			Effects->Shake = GL_FALSE;
	}
	UpdatePowerUps(dt);
}

void Game::Render()
{
	if (State == GAME_ACTIVE) {
		Effects->BeginRender();
		//���Ʊ���
		Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(Width, Height), 0.0f);
		//���ƹؿ�
		levels[level].Draw(*Renderer);
		//���ƽ�ɫ����
		Player->Draw(*Renderer);
		//��������
		Particles->Draw();
		//������
		Ball->Draw(*Renderer);
		for (auto powerUp : PowerUps)
			if (!powerUp.Destroyed)
				powerUp.Draw(*Renderer);

		Effects->EndRender();
		Effects->Render(glfwGetTime());
	}
}

void Game::DoCollisions()
{
	//����ש�����ײ
	for (GameObject& box : levels[level].Bricks) {
		if (!box.Destroyed) {
			Collision collision = CheckCollision(*Ball, box);
			if (std::get<0>(collision)) {//�����ײ���Ļ�
				//���ש�鲻��ʵ�ľ�����
				if (!box.IsSolid) {
					box.Destroyed = GL_TRUE;
					SpawnPowerUps(box);
				}
				else {
					//�����ʵ�����򴥷�shake��Ч
					ShakeTime = 0.05f;
					Effects->Shake = GL_TRUE;
				}
				//�������Դ����Ͳ���Ҫ��ײ����
				if (Ball->PassThrough) continue;
				//��ײ����
				Direction dir = std::get<1>(collision);
				glm::vec2 diff_vector = std::get<2>(collision);
				//ˮƽ��ײ
				if (dir == LEFT || dir == RIGHT) {
					//��תˮƽ�ٶ�
					Ball->Velocity.x = -Ball->Velocity.x;
					//�ض�λ����������ש����
					GLfloat penetration = Ball->Radius - glm::length(diff_vector);
					if (dir == LEFT)
						Ball->Position.x += penetration;
					else
						Ball->Position.x -= penetration;
				}
				//��ֱ��ײ
				else {
					Ball->Velocity.y = -Ball->Velocity.y;
					GLfloat penetration = Ball->Radius - glm::length(diff_vector);
					if (dir == UP)
						Ball->Position.y += penetration;
					else
						Ball->Position.y -= penetration;
				}
			}
		}
	}
	//�����ɫ�������ײ
	Collision collision = CheckCollision(*Ball, *Player);
	//��û�й̶����ҷ�����ײ
	if (!Ball->Stuck && std::get<0>(collision)) {
		//���ݵ��߸���״̬
		Ball->Stuck = Ball->Sticky;
		GLfloat centerBoard = Player->Position.x + Player->Size.x / 2;
		//���Բ�ľ൲�����ĵ�ˮƽ����
		GLfloat distence = (Ball->Position.x + Ball->Radius) - centerBoard;
		GLfloat percentage = distence / (Player->Size.x / 2);

		GLfloat strength = 2.0f;
		glm::vec2 oldVelocity = Ball->Velocity;
		Ball->Velocity.x = BALL_VELOCITY.x * percentage * strength;
		//��Ϊ����ײ����֮��y����ٶȱ�Ȼ�����ϵ�
		Ball->Velocity.y = -std::abs(Ball->Velocity.y);
		//��֤����ٶ�ʸ������
		Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
	}
	//��ɫ��������ߵ���ײ
	for (auto& powerUp : PowerUps) {
		if (!powerUp.Destroyed) {
			//������������
			if (powerUp.Position.y >= Height)
				powerUp.Destroyed = GL_TRUE;
			//ʹ��AABB��ײ���
			if (CheckCollision(*Player, powerUp)) {
				ActivatePowerUp(powerUp);
				powerUp.Destroyed = GL_TRUE;
				powerUp.Activated = GL_TRUE;
			}
		}
	}
}

GLboolean Game::CheckCollision(GameObject& one, GameObject& two)
{
	// x�᷽����ײ
	bool collisionX = one.Position.x + one.Size.x >= two.Position.x && two.Position.x + two.Size.x >= one.Position.x;
	// y�᷽����ײ
	bool collisionY = one.Position.y + one.Size.y >= two.Position.y && two.Position.y + two.Size.y >= one.Position.y;
	// ֻ���������򶼷�����ײʱ������ײ
	return collisionX && collisionY;
}

Collision Game::CheckCollision(BallObject& one, GameObject& two)
{
	//��ȡԲ��
	glm::vec2 center(one.Position + one.Radius);
	//��ȡAABB����Ϣ
	glm::vec2 aabb_half_extents(two.Size.x / 2, two.Size.y / 2);
	glm::vec2 aabb_center(two.Position + aabb_half_extents);
	//��ȡ�������ĵ�ʸ����
	glm::vec2 difference = center - aabb_center;
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
	//���ļ���clamped����AABB�Ͼ���Բ����ĵ�
	glm::vec2 closest = aabb_center + clamped;
	difference = center - closest;
	//�������С�ڰ뾶��˵��ײ����
	//������ϵ��ڵĻ�������ʵ��������֮���λ�û�������ײ��Χ��
	if (glm::length(difference) < one.Radius)
		return std::make_tuple(GL_TRUE, VectorDirection(difference), difference);
	else
		return std::make_tuple(GL_FALSE, UP, glm::vec2(0.0f, 0.0f));
}

Direction Game::VectorDirection(glm::vec2 target)
{
	/*
	��ʾ������ײ����������ĸ�����
	target ����ײ������(��Բ�ĳ���)�� ��������ɰ���ײ������ƽ�Ƶ����Ͻ�ԭ�㣬Ȼ�������Ͻ�ԭ���϶���4������Ȼ������������ĵ�˽���ж��ĸ���ײ���������ĸ����������
	max Խ��˵���н�ԽС��˵��Բ�ĵ������ķ�������Խ�����ĸ�����
	*/
	glm::vec2 compass[] = {
		glm::vec2(0.0f,1.0f), //��
		glm::vec2(0.0f,-1.0f),//��
		glm::vec2(1.0f,0.0f), //��
		glm::vec2(-1.0f,0.0f) //��
	};
	GLfloat max = 0.0f;
	GLuint best_match = -1;
	for (GLuint i = 0;i < 4;i++) {
		GLfloat dot_product = glm::dot(glm::normalize(target), compass[i]);
		if (dot_product > max) {
			max = dot_product;
			best_match = i;
		}
	}
	return Direction(best_match);
}

void Game::ResetLevel()
{
	if (level == 0) levels[0].Load("res/levels/one.txt", Width, Height * 0.5f);
	else if (level == 1) levels[1].Load("res/levels/two.txt", Width, Height * 0.5f);
	else if (level == 2) levels[2].Load("res/levels/three.txt", Width, Height * 0.5f);
	else if (level == 3) levels[3].Load("res/levels/four.txt", Width, Height * 0.5f);
}

void Game::ResetPlayer()
{
	Player->Size = PLAYER_SIZE;
	Player->Position = glm::vec2(Width / 2 - PLAYER_SIZE.x / 2, Height - PLAYER_SIZE.y);
	Ball->Reset(glm::vec2(Player->Position + glm::vec2(PLAYER_SIZE.x / 2 - Ball->Radius, -Ball->Radius * 2)),BALL_VELOCITY);
}

void Game::SpawnPowerUps(GameObject& Object)
{
	GLuint chance = 75;
	GLuint random = rand() % chance;
	//�����������
	switch (random) {
	case 0:
		PowerUps.push_back(PowerUp(SPEED, 0.0f, glm::vec3(0.5f, 0.5f, 1.0f), Object.Position, ResourceManager::GetTexture(("speed"))));
		break;
	case 1:
		PowerUps.push_back(PowerUp(STICKY, 20.0f, glm::vec3(1.0f, 0.5f, 1.0f), Object.Position, ResourceManager::GetTexture(("sticky"))));
		break;
	case 2:
		PowerUps.push_back(PowerUp(PASS_THROUGH, 10.0f, glm::vec3(0.5f, 1.0f, 0.5f), Object.Position, ResourceManager::GetTexture(("passthrough"))));
		break;
	case 3:
		PowerUps.push_back(PowerUp(PAD_SIZE_INCREASE, 0.0f, glm::vec3(1.0f, 0.6f, 0.4f), Object.Position, ResourceManager::GetTexture(("increase"))));
		break;
	default:
		break;
	}
	//�����������
	random = rand() % 15;
	switch (random)
	{
	case 0:
		PowerUps.push_back(PowerUp(CHAOS, 15.0f, glm::vec3(1.0f, 0.3f, 0.3f), Object.Position, ResourceManager::GetTexture("chaos")));
		break;
	case 1:
		PowerUps.push_back(PowerUp(CONFUSE, 15.0f, glm::vec3(0.9f, 0.25f, 0.25f), Object.Position, ResourceManager::GetTexture("confuse")));
		break;
	default:
		break;
	}
}

void Game::UpdatePowerUps(GLfloat dt)
{
	for (auto& powerUp : PowerUps) {
		powerUp.Position += powerUp.Velocity * dt;
		if (powerUp.Activated) {
			powerUp.Duration -= dt;
			//����ʱ�����ͣ��Ч��
			//�����ͬ����������оͲ���Ҫͣ��
			if (powerUp.Duration <= 0.0f) {
				powerUp.Activated = GL_FALSE;
				switch (powerUp.Type)
				{
				case STICKY:
					if (!IsOtherPowerActive(PowerUps, STICKY)) {
						Ball->Sticky = GL_FALSE;
						Player->Color = glm::vec3(1.0f);
					}
					break;
				case PASS_THROUGH:
					if (!IsOtherPowerActive(PowerUps, PASS_THROUGH)) {
						Ball->PassThrough = GL_FALSE;
						Ball->Color = glm::vec3(1.0f);
					}
					break;
				case CONFUSE:
					if (!IsOtherPowerActive(PowerUps, CONFUSE)) 
						Effects->Confuse = GL_FALSE;
					break;
				case CHAOS:
					if (!IsOtherPowerActive(PowerUps, CHAOS))
						Effects->Chaos = GL_FALSE;
				default:
					break;
				}
			}
		}
	}
	//�Ƴ��Ѿ���������û���ڻ�ĵ���
	PowerUps.erase(std::remove_if(PowerUps.begin(), PowerUps.end(),
		[](const PowerUp& powerUp) { return powerUp.Destroyed && !powerUp.Activated; }
	), PowerUps.end());
}

void Game::ActivatePowerUp(PowerUp& powerUp)
{
	switch (powerUp.Type)
	{
	case SPEED:
		Ball->Velocity *= 1.2f;
		break;
	case STICKY:
		Ball->Sticky = GL_TRUE;
		Player->Color = glm::vec3(1.0f, 0.5, 1.0f);
		break;
	case PASS_THROUGH:
		Ball->PassThrough = GL_TRUE;
		Ball->Color = glm::vec3(1.0f, 0.5f, 0.5f);
		break;
	case PAD_SIZE_INCREASE:
		Player->Size.x += 50;
		break;
	case CHAOS:
		//ֻ��δ����ʱ��Ч
		if (!Effects->Chaos)
			Effects->Chaos = GL_TRUE;
	case CONFUSE:
		if (!Effects->Confuse)
			Effects->Confuse = GL_TRUE;
	default:
		break;
	}
}

GLboolean Game::IsOtherPowerActive(std::vector<PowerUp>& powerUps, PROPTYPE type)
{
	for (const auto powerUp : powerUps) {
		if (powerUp.Type == type && powerUp.Activated)
			return GL_TRUE;
	}
	return GL_FALSE;
}
