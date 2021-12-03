#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <cstdlib>
#include <stdlib.h>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <locale>
#include <ctime>

/*
	_"ASTEROID BLASTER" GAME RULES_

* Keep blasting until countdown ends!
* Use the SPACE bar to SHOOT the asteroids with your lasers
* Use the LEFT and RIGHT keys to MOVE the blaster character
*/

/*
* 
* Gavin Kelly			- (Latest Update - int main() + void classes + functions)
* Matthew Mulligan		- (Latest Update - Created Scoreboard + Timer Properties)
* Matthew Devine		- (Latest Update - Found audio & image files + Created Game Over screen)
*/
using namespace sf;

//Functions for the Blaster's movement on the x axis given, LaserBlast & Asteroid Spawner
void BlasterMovement(int &KeyTimes, CircleShape &Blaster, RenderWindow &window); 
void LaserBlast(int& LaserBallTimer, Sound& Laser, CircleShape& Asteroid, std::vector<CircleShape>& Asteroids, Vector2f& BlasterCenter);
void AsteroidSpawner(int& AsteroidSpawnTimer, RectangleShape& Asteroid, std::vector<RectangleShape>& Asteroids, RenderWindow& window);
void StarSpawner(int& StarSpawnTimer, CircleShape& Star, std::vector<CircleShape>& Stars, RenderWindow& window, RectangleShape& Asteroid);

int main()
{
	//Setting Scoreboard & Countdown
	int Score = 0;
	int Countdown = 51;
	Clock clock;
	std::string countdownString;
	std::ostringstream convert;
	convert << Countdown;
	countdownString = convert.str();

	Text timerText;
	Text Scoreboard;
	Text FINALSCORE;
	Font arial;
	arial.loadFromFile("ArialCE.ttf");

	//Setting Background Image
	Texture BackgroundTexture;
	BackgroundTexture.loadFromFile("SpaceBackgroundGame.jpg");
	Sprite BackgroundSprite;
	Vector2u size = BackgroundTexture.getSize();
	BackgroundSprite.setTexture(BackgroundTexture);  //This is used to set the image to the sprite
	BackgroundSprite.setOrigin(size.x / 2, size.y / 2);

	//Setting Game Over Screen
	Texture GameOverTexture;
	GameOverTexture.loadFromFile("GameOver.jpg");
	Sprite GameOverSprite;
	Vector2u size2 = GameOverTexture.getSize();
	GameOverSprite.setTexture(GameOverTexture); //This is used to set the image to the sprite
	GameOverSprite.setOrigin(80,0);


		//Audio for laser, bombs & music
		SoundBuffer LaserBuffer, BombBuffer, MusicBuffer, GameOverBuffer;

		if (!LaserBuffer.loadFromFile("Laser.ogg"))
			return -1;

		if (!BombBuffer.loadFromFile("BombSound.ogg"))
			return -1;

		if (!MusicBuffer.loadFromFile("Music.ogg"))
			return -1;

		if (!GameOverBuffer.loadFromFile("GameOverSound.ogg"))
			return -1;


		Sound Laser, Bomb, Music, GameOver1;
		Laser.setBuffer(LaserBuffer);
		Bomb.setBuffer(BombBuffer);
		Music.setBuffer(MusicBuffer);
		GameOver1.setBuffer(GameOverBuffer);
		Music.play();

	int KeyTimes = 5;		//This is the number of KeyFrames that are needed for the game

	srand(time(NULL));		//This is needed to create a Random Seed for each game 

	sf::RenderWindow window(sf::VideoMode(640, 480), "Asteroid Blaster 2021");
	window.setFramerateLimit(60);		//This sets the frame rate for the game

				//Declaring the objects in the game

	CircleShape LaserBall;
	LaserBall.setFillColor(sf::Color(237, 242, 103)); // Yellow
	LaserBall.setRadius(3.f);

	CircleShape Star;
	Star.setFillColor(Color::White);
	Star.setRadius(1.f);

	RectangleShape Asteroid;
	Asteroid.setFillColor(sf::Color(92, 60, 60)); // Brown
	Asteroid.setSize(Vector2f(50.f, 50.f));

	CircleShape Blaster;
	Blaster.setFillColor(sf::Color(82, 239, 245)); // Light Blue
	Blaster.setRadius(30.f);
	Blaster.setOrigin(Blaster.getRadius(), Blaster.getRadius());
	Blaster.setPosition(window.getSize().x / 2, window.getSize().y- 70);
	Vector2f BlasterCenter;
	int LaserBallTimer = 0;

					//This is used to create the lasers, the background stars and the asteroids  
	std::vector<CircleShape> LaserBalls;
	LaserBalls.push_back(CircleShape(LaserBall));

	std::vector<RectangleShape> Asteroids;
	Asteroids.push_back(RectangleShape(Asteroid));
	int AsteroidSpawnTimer = 0;

	std::vector<CircleShape> Stars;
	Stars.push_back(CircleShape(Star));
	int StarSpawnTimer = 0;


					//EVENTS

	sf::Event event;

	while (window.isOpen())
	{
		//Updating Scoreboard Constantly
		std::stringstream ss;
		Scoreboard.setCharacterSize(25);
		Scoreboard.setPosition({ 10, 10 });
		Scoreboard.setFont(arial);
		Scoreboard.setFillColor(Color::Green);
        ss << "Score: " << Score;
        Scoreboard.setString(ss.str());

		//Updating Timer constantly every second
		timerText.setCharacterSize(25);
		timerText.setPosition({ 450, 10 });
		timerText.setFont(arial);
		timerText.setFillColor(Color::Green);

		//FINAL SCORE
		std::stringstream FS;
		FINALSCORE.setCharacterSize(25);
		FINALSCORE.setPosition({ 100, 10 });
		FINALSCORE.setFont(arial);
		FINALSCORE.setFillColor(Color::Green);
		FS << "Final ";
		FINALSCORE.setString(FS.str());

		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();

			}
		}

		//UPDATES

		//blaster events
		BlasterCenter = Vector2f(Blaster.getPosition().x, Blaster.getPosition().y); //Used for finding the Centre Area of the Blaster
		BlasterMovement(KeyTimes, Blaster, window);

		//laser events
		LaserBlast(LaserBallTimer, Laser, LaserBall, LaserBalls, BlasterCenter);

		//Asteroid events
		AsteroidSpawner(AsteroidSpawnTimer, Asteroid, Asteroids, window);

		//Star events
		StarSpawner(StarSpawnTimer, Star, Stars, window, Asteroid);

		//Collision events
		// 
		//Used when the Laser ball hits the Asteroid, the asteroid circle will be "destroyed"
		if (!Asteroids.empty() && !LaserBalls.empty())
		{
			for (size_t i = 0; i < LaserBalls.size(); i++)
			{
				for (size_t j = 0; j < Asteroids.size(); j++)
				{
					if (LaserBalls[i].getGlobalBounds().intersects(Asteroids[j].getGlobalBounds()))
					{
						LaserBalls.erase(LaserBalls.begin() + i);
						Asteroids.erase(Asteroids.begin() + j);
						Score = Score + 100;
						Bomb.play();
						break;
					}
				}
			}
		}

		//Close Game event

		if (Keyboard::isKeyPressed(sf::Keyboard::Escape))
		{
			// Closes the application
			window.close();
		}

		//DRAWING SHAPES TO SCREEN
		window.clear();
		window.draw(BackgroundSprite);
		window.draw(Blaster);

		//DRAWS ASTEROIDS
		for (size_t i = 0; i < Asteroids.size(); i++)
		{
			window.draw(Asteroids[i]);
		}

		//DRAWS STARS
		for (size_t i = 0; i < Stars.size(); i++)
		{
			window.draw(Stars[i]);
		}

		//DRAWS LASERS
		for (size_t i = 0; i < LaserBalls.size(); i++)
		{
			window.draw(LaserBalls[i]);
		}
		window.draw(Scoreboard);
		window.draw(timerText);

		int timer = clock.getElapsedTime().asSeconds();
		std::cout << timer << std::endl;

		if (timer > 0) {
			Countdown--;
			timerText.setString("Countdown: " + std::to_string(Countdown));
			clock.restart();
		}
		if (Countdown < 1) {
			window.draw(GameOverSprite);
			FINALSCORE.setFillColor(Color::White);
			Scoreboard.setFillColor(Color::White);
			FINALSCORE.setPosition({ 250, 420 });
			Scoreboard.setPosition({ 310, 420 });
			window.draw(FINALSCORE);
			window.draw(Scoreboard);
			Music.stop();
			Bomb.stop();
			Laser.stop();	
		}
		if (Countdown == 1) { GameOver1.play(); }
		window.display();
	}
	return 0; 
}


//MOVEMENT FUNCTIONS

//Using Left and Right Keys to move Blaster character
void BlasterMovement(int& KeyTimes, CircleShape& Blaster, RenderWindow &window)
{
	if (KeyTimes < 5)
		KeyTimes++;

	if (Keyboard::isKeyPressed(Keyboard::Left) && Blaster.getPosition().x - Blaster.getRadius() > 0 ) {
		Blaster.move(-8.f, 0.f);
		KeyTimes = 0;
	}

	if (Keyboard::isKeyPressed(Keyboard::Right) && Blaster.getPosition().x +Blaster.getRadius() < window.getSize().x) {
		Blaster.move(8.f, 0.f);
		KeyTimes = 0;
	}
} 

void LaserBlast(int& LaserBallTimer, Sound& Laser, CircleShape& Asteroid, std::vector<CircleShape>& Asteroids, Vector2f& BlasterCenter) //Shooting Function
{
	if (LaserBallTimer < 6)
		LaserBallTimer++;
	//Shoot the laser using the Space Bar button
	if (Keyboard::isKeyPressed(Keyboard::Space) && LaserBallTimer >= 6)
	{
		Asteroid.setPosition(BlasterCenter);
		Asteroids.push_back(CircleShape(Asteroid));

		Laser.play();

		LaserBallTimer = 2;
	}

	for (size_t i = 0; i < Asteroids.size(); i++)
	{
		Asteroids[i].move(0.f, -8.f);

		if (Asteroids[i].getPosition().y <= 0)
			Asteroids.erase(Asteroids.begin() + i);
	}
}

void AsteroidSpawner(int& AsteroidSpawnTimer, RectangleShape& Asteroid, std::vector<RectangleShape>& Asteroids, RenderWindow& window)
{
	//Spawns a maximum of 20 asteroids to the screen
	if (AsteroidSpawnTimer < 20)
		AsteroidSpawnTimer++;

	if (AsteroidSpawnTimer >= 20)
	{
		//This is used to make the asteroids randomly spawn anywhere from the top of the screen
		Asteroid.setPosition((rand() % int(window.getSize().x - Asteroid.getSize().x)), 0.f);
		Asteroids.push_back(RectangleShape(Asteroid));

		AsteroidSpawnTimer = 0;
	}

	//This is used to move the asteroids downwards on the screen
	for (size_t i = 0; i < Asteroids.size(); i++)
	{
		Asteroids[i].move(0.f, 4.f);

		if (Asteroids[i].getPosition().y > window.getSize().y)
			Asteroids.erase(Asteroids.begin() + i);
	}
}

void StarSpawner(int& StarSpawnTimer, CircleShape& Star, std::vector<CircleShape>& Stars, RenderWindow& window, RectangleShape& Asteroid)
{
	//Spawns many stars to the background of the screen
	if (StarSpawnTimer < 5)
		StarSpawnTimer++;

	if (StarSpawnTimer >= 5)
	{
		//This is used to make the asteroids randomly spawn anywhere from the top of the screen
		Star.setPosition((rand() % int(window.getSize().x - Asteroid.getSize().x)), 0.f);
		Stars.push_back(CircleShape(Star));

		StarSpawnTimer = 0;
	}

	//This is used to move the asteroids downwards on the screen
	for (size_t i = 0; i < Stars.size(); i++)
	{
		Stars[i].move(0.f, 20.f);

		if (Stars[i].getPosition().y > window.getSize().y)
			Stars.erase(Stars.begin() + i);
	}
}
