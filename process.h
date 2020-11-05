#include "stdafx.h"

// standard
#include <assert.h>
#include <math.h>

// glut
#include <GL/glut.h>

//number of balls
static int ball_number = 5;

//initial position of balls
GLfloat ini_position[5][3] = {
	{ 1.0f, 5.0f, 0.0f } ,
	{ -1.0f, 4.0f, 1.0f },
	{ 5.0f, 2.0f, 1.0f },
	{ 5.0f, 9.0f, 0.0f },
	{ 6.0f, 8.0f, 4.0f },
};

//initial speed of balls
GLfloat speed[5][3] = {
	{ 1, 0, 0 },
	{ 1, 0, 0 },
	{ -1, 0, 0 },
	{ -1, 0, 0 },
	{ 0, 0, 0 },
};

//ball position
GLfloat ball_position[5][3] = { 0 };

//energyloss after collision
GLfloat energyloss = 0.9f;

//time
GLfloat time = 0.05f;

//gravity which only effect y axis
GLfloat gravity[3] = { 0,-0.98,0 };

//create matrix for balls
GLfloat ball_matrix[5][4][4];

// initial vector
void init_vector(GLfloat vector[3]) {
	GLfloat base = sqrt(vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2]);
	if (base != 0)
	{
		vector[0] = vector[0] / base;
		vector[1] = vector[1] / base;
		vector[2] = vector[2] / base;
	}
}

//vector cross product
void crossproduct(GLfloat a[3], GLfloat b[3], GLfloat result[3])
{
	result[0] = a[1] * b[2] - a[2] * b[1];
	result[1] = a[2] * b[0] - a[0] * b[2];
	result[2] = a[0] * b[1] - a[1] * b[0];
}

//vector dot product
GLfloat dotproduct(GLfloat vector1[3], GLfloat vector2[3])
{
	GLfloat result = 0;
	for (int i = 0; i < 3; i++) {
		result += vector1[i] + vector2[i];
	}
	return result;
}

//distance
GLfloat distance(GLfloat ball1[3], GLfloat ball2[3])
{
	GLfloat distance = 0;
	GLfloat temp = 0;
	for (int i = 0; i < 3; i++) {
		temp += pow(ball1[i] - ball2[i], 2);
	}
	distance = sqrt(temp);
	return distance;
}

//Rebound after contact with the floor
void rebound(int ball) {
	if (ball_position[ball][1] < 0.5) // Floor Collision Happens
	{
		// due to energy loss, the speed will decrease
		// the direction will change
		speed[ball][1] *= -energyloss;

		// x,z axis will not be effected
		speed[ball][0] = speed[ball][0];
		speed[ball][2] = speed[ball][2];
	}
	//left&right walls?
	//front&behind walls?
}

//collision checker
void collision(int currentball) {
	for (int i = currentball + 1; i < ball_number; i++)
	{
		if (distance(ball_position[currentball], ball_position[i]) < 1.0) // check the distance between balls
		{
			// calculate the x_axis for current ball
			GLfloat x_axis[3];
			for (int j = 0; j < 3; j++)
			{
				x_axis[j] = ball_position[i][j] - ball_position[currentball][j];
			}
			init_vector(x_axis);

			// calculate the speed of current ball on x-axis and y-axis 
			GLfloat curball_x[3], curball_y[3];
			GLfloat current_result = dotproduct(x_axis, speed[currentball]);
			for (int j = 0; j < 3; j++)
			{
				curball_x[j] = current_result * x_axis[j];
				curball_y[j] = speed[currentball][j] - curball_x[j];
			}

			// calculate the x_axis for the other ball
			for (int j = 0; j < 3; j++)
			{
				x_axis[j] = ball_position[currentball][j] - ball_position[i][j];
			}
			init_vector(x_axis);

			// calculate the speed of collided ball on x-axis and y-axis 
			GLfloat colball_x[3], colball_y[3];
			GLfloat collided_result = dotproduct(x_axis, speed[i]);
			for (int j = 0; j < 3; j++)
			{
				colball_x[j] = collided_result * x_axis[j];
				colball_y[j] = speed[i][j] - colball_x[j];
			}

			// calculate the speed
			GLfloat curball_speed[3], colball_speed[3];
			for (int j = 0; j < 3; j++)
			{
				curball_speed[j] = (curball_x[j] - (curball_x[j] - colball_x[j])) * energyloss;
				colball_speed[j] = (colball_x[j] - (colball_x[j] - curball_x[j])) * energyloss;
				speed[currentball][j] = curball_speed[j] + curball_y[j];
				speed[i][j] = colball_speed[j] + colball_y[j];
			}
			continue;
		}
	}
}

//movement
void movement(int currentball) {
	rebound(currentball);
	collision(currentball);
	GLfloat nextposition[5][3];
	GLfloat nextspeed[5][3];

	for (int i = 0; i < 3; i++) {
		// v = v0 + at
		nextspeed[currentball][i] = speed[currentball][i] + gravity[i] * time;
		speed[currentball][i] = nextspeed[currentball][i];
		// s = s + vt
		nextposition[currentball][i] = ball_position[currentball][i] + speed[currentball][i] * time;
		ball_position[currentball][i] = nextposition[currentball][i];
		//load ball model matrix
		ball_matrix[currentball][3][i] = ball_position[currentball][i];
	}

}

//render balls
void ballrender() {
	GLfloat temp[16] = { 0 };
	for (int i = 0; i < ball_number; i++) {
		glPushMatrix();
		movement(i);
		for (int m = 0; m < 4; m++) {
			for (int n = 0; n < 4; n++) {
				temp[(4 * m) + n] = ball_matrix[i][m][n];
			}
		}
		glMultMatrixf(temp);
		glColor3f(0.0, 0.0, 1.0);
		glutSolidSphere(0.5, 10, 10);
		glPopMatrix();
	}
}

//Initial Ball matrix
void ini_matrix() {
	for (int i = 0; i < ball_number; i++)
	{
		ball_matrix[i][0][0] = 1.0f;
		ball_matrix[i][1][1] = 1.0f;
		ball_matrix[i][2][2] = 1.0f;
		ball_matrix[i][3][3] = 1.0f;
		for (int j = 0; j < 3; j++)
		{
			ball_matrix[i][3][j] = ini_position[i][j];
			ball_position[i][j] = ini_position[i][j];
		}
	}
}

void floor() {
	GLfloat edge = -10.0f;
	for (GLfloat x = -100; x < 100; x += 2.0f)
	{
		glColor3f(0.0, 0.0, 1);
		glVertex3f(x, 0, -100);
		glVertex3f(x, 0, 100);
		glVertex3f(x, -100, edge);
		glVertex3f(x, 100, edge);
	}
	for (GLfloat z = -150; z < 100; z += 2.0f)
	{
		glColor3f(0.0, 0.0, 1);
		glVertex3f(-150, 0, z);
		glVertex3f(100, 0, z);
		glVertex3f(-100, z, edge);
		glVertex3f(100, z, edge);
	}

	glEnd();
	
}





#pragma once
