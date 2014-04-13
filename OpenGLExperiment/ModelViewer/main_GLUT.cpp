#include <fstream>
#include <iostream>
#include <gl\glut.h>

#include <DirectXMath.h>
#include "SimpleMath.h"

#include "ObjModel.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

int  winWidth, winHeight;
int		g_CurrentGLUTWindow;

bool	g_TrackingMouse = false;
bool	g_Translating = false;

std::vector<Models::RigidObjModel> g_Models;	// The models we load from obj files.
size_t		g_CurrentModelIdx = 0;				// The index of current model to be draw
Vector3		g_RotationStartPoint;					// Record the last mouse postion for impleament the rotation operation
Vector3		g_TranslationVelocity;
/* Draw the cube */
GLfloat vertices [][3] = {
		{ -1.0, -1.0, -1.0 }, { 1.0, -1.0, -1.0 }, { 1.0, 1.0, -1.0 },
		{ -1.0, 1.0, -1.0 },
		{ -1.0, -1.0, 1.0 }, { 1.0, -1.0, 1.0 }, { 1.0, 1.0, 1.0 }, { -1.0, 1.0, 1.0 }
};
GLfloat colors [][3] = {
		{ 0.0, 0.0, 0.0 }, { 1.0, 0.0, 0.0 }, { 1.0, 1.0, 0.0 }, { 0.0, 1.0, 0.0 },
		{ 0.0, 0.0, 1.0 }, { 1.0, 0.0, 1.0 }, { 1.0, 1.0, 1.0 }, { 0.0, 1.0, 1.0 }
};
void polygon(int a, int b, int c, int d, int face)
{
	/* draw a polygon via list of vertices */
	glBegin(GL_POLYGON);
	glColor3fv(colors[a]);
	glVertex3fv(vertices[a]);
	glVertex3fv(vertices[b]);
	glVertex3fv(vertices[c]);
	glVertex3fv(vertices[d]);
	glEnd();
}
void colorcube(void)
{
	/* map vertices to faces */
	polygon(1, 0, 3, 2, 0);
	polygon(3, 7, 6, 2, 1);
	polygon(7, 3, 0, 4, 2);
	polygon(2, 6, 5, 1, 3);
	polygon(4, 5, 6, 7, 4);
	polygon(5, 4, 0, 1, 5);
}

void SetPerspectiveProjection()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();									// Reset The Projection Matrix
	gluPerspective(45.0f, (GLfloat) winWidth / (GLfloat) winHeight, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
}

void SetOrthoProjection()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();								// Reset The Projection Matrix
	float aspect = (GLfloat) winWidth / (GLfloat) winHeight;
	glOrtho(-aspect, aspect, -1.0, 1.0, 0.1f, 100);
	glMatrixMode(GL_MODELVIEW);
}

Vector3	ProjectScreenIntoUintSphere(XMUINT2 pos, uint32_t width, uint32_t height)
{
	Vector3 v;
	/* project x,y onto a hemisphere centered within width, height */
	v.x = (2.0f*pos.x - width) / width;
	v.y = (height - 2.0f*pos.y) / height;
	v.z = sqrt(1.0f - v.LengthSquared());
	return v;
}

void OnMouseMotion_TrackingRotation(int x, int y)
{
	auto endSpherePoint = ProjectScreenIntoUintSphere(XMUINT2( x, y ), winWidth, winHeight);
	auto delta = (endSpherePoint - g_RotationStartPoint);
	float angle = delta.Length();
	auto axis = g_RotationStartPoint.Cross(endSpherePoint);
	if (axis.LengthSquared() > 0.001f)
	{
		//g_Models[g_CurrentModelIdx].Orientation = DirectX::XMQuaternionMultiply((XMVECTOR)initialOrientation,XMQuaternionRotationNormal(axis, angle));
		g_Models[g_CurrentModelIdx].Orientation *= XMQuaternionRotationAxis(axis, angle);
		g_RotationStartPoint = endSpherePoint;
	}

	glutPostRedisplay();
}
void RotationTracking_Begin(int x, int y)
{
	g_TrackingMouse = true;
	g_RotationStartPoint = ProjectScreenIntoUintSphere(XMUINT2( x, y ), winWidth, winHeight);
}
void RotationTracking_End(int x, int y)
{
	g_TrackingMouse = false;

}

bool RenderObjModelWithVertexIndexArray(const Models::RigidObjModel& model)
{
	glPushMatrix();
	// Setup the model's rigid transformation
	Matrix mat = model.GetModelMatrix();
	glMultMatrixf((float*) &mat);

	const auto& vertices = static_cast<const VertexCollection<VertexTypes::VertexPositionColor>&>(model.Vertices());
	const auto& facts = model.Facets();
	const uint32_t* indices = &facts[0].x;

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(3, GL_FLOAT, 7 * sizeof(GLfloat), &vertices[0].Position);
	glColorPointer(3, GL_FLOAT, 7 * sizeof(GLfloat), &vertices[0].Color);

	//glBegin(GL_TRIANGLES);						

	glDrawElements(GL_TRIANGLES, facts.size() * 3, GL_UNSIGNED_INT, indices);

	//glEnd();				

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glPopMatrix();
	return true;
}

void RenderVertexPositionColor(const VertexTypes::VertexPositionColor& v)
{
	glVertex3f(v.Position.x, v.Position.y, v.Position.z);
	glColor3f(v.Color.x, v.Color.y, v.Color.z);
}

bool RenderObjModelToDisplayList(const Models::RigidObjModel& model)
{
	glPushMatrix();
	// Setup the model's rigid transformation
	Matrix mat = model.GetModelMatrix();
	glMultMatrixf((float*) &mat);

	//glBegin(GL_TRIANGLES);
	glNewList(g_CurrentModelIdx, GL_COMPILE);
	const auto& vertices = static_cast<const VertexCollection<VertexTypes::VertexPositionColor>&>(model.Vertices());
	const auto& facts = model.Facets();
	for (const auto& f : facts)
	{
		RenderVertexPositionColor(vertices[f.x]);
		RenderVertexPositionColor(vertices[f.y]);
		RenderVertexPositionColor(vertices[f.z]);
	}
	glEnd();

	glPopMatrix();
	return true;
}

void OnSceneRedrawing(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	RenderObjModelWithVertexIndexArray(g_Models[g_CurrentModelIdx]);

	//glCallList(g_CurrentModelIdx);

	glutSwapBuffers();
}
void OnMouseButtonStateChanged(int button, int state, int x, int y)
{
	if (button == GLUT_RIGHT_BUTTON) exit(0);
	if (button == GLUT_LEFT_BUTTON) switch (state)
	{
	case GLUT_DOWN:
		y = winHeight - y;
		RotationTracking_Begin(x, y);
		break;
	case GLUT_UP:
		RotationTracking_End(x, y);
		break;
	}
}
void OnWindowResizing(int w, int h)
{
	glViewport(0, 0, w, h);
	winWidth = w;
	winHeight = h;
	SetPerspectiveProjection();
}

bool LoadAssetes()
{
	std::ifstream cow("cow_color.obj");
	if (!cow.is_open())
		return false;
	g_Models.emplace_back((istream&) cow);
	g_Models.back().Position.z = -2.0f;

	std::ifstream v1("v1_color.obj");
	if (!v1.is_open())
		return false;
	g_Models.emplace_back(v1);
	g_Models.back().Scale = Vector3(0.1f, 0.1f, 0.1f);
	g_Models.back().Position.z = -2.0f;
	return true;
}

void OnKeyPressed(unsigned char key, int x, int y)
{
	switch (key){

		// Continues keystrokes
	case 'x':
	{
		g_TranslationVelocity.z = 0.1f;
		break;
	}
	case 'z':
	{
		g_TranslationVelocity.z = -0.1f;
		break;
	}

	default:
		break;
	}
}

// We handle arrow keys here because it cannot be expressed by ASCII code
void OnSpecialKeyPressed(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
	{
		g_TranslationVelocity.y = 0.1f;
		break;
	}
	case GLUT_KEY_DOWN:
	{
		g_TranslationVelocity.y = -0.1f;
		break;
	}
	case GLUT_KEY_LEFT:
	{
		g_TranslationVelocity.x = -0.1f;
		break;
	}
	case GLUT_KEY_RIGHT:
	{
		g_TranslationVelocity.x = 0.1f;
		break;
	}
	default:
		break;
	}
}
void OnKeyReleased(unsigned char key, int x, int y)
{
	glutPostRedisplay();

	switch (key){
		case 27:
		{
			// God dame GLUT
			//glutLeaveMainLoop();
			glutDestroyWindow(g_CurrentGLUTWindow);
			exit(0);
		}

		case 'o':
		{
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();								// Reset The Projection Matrix
			float aspect = (GLfloat) winWidth / (GLfloat) winHeight;
			glOrtho(-aspect, aspect, -1.0, 1.0, 0.1f, 100);
			glMatrixMode(GL_MODELVIEW);
			break;
		}

		case 'p':
		{
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();									// Reset The Projection Matrix
			gluPerspective(45.0f, (GLfloat) winWidth / (GLfloat) winHeight, 0.1f, 100.0f);
			glMatrixMode(GL_MODELVIEW);
			break;
		}
		case '1':
		{
			g_CurrentModelIdx = 0;
			break;
		}
		case '2':
		{
			g_CurrentModelIdx = 1;
			break;
		}

			// Continues keystrokes
		case 'z':
		case 'x':
		{
			g_TranslationVelocity.z = 0.0f;
			break;
		}
		default:
			break;
	}
}

// We handle arrow keys here because it cannot be expressed by ASCII code
void OnSpecialKeyReleased(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
	case GLUT_KEY_DOWN:
	{
		g_TranslationVelocity.y = 0.0f;
		break;
	}

	case GLUT_KEY_LEFT:
	case GLUT_KEY_RIGHT:
	{
		g_TranslationVelocity.x = 0.0f;
		break;
	}
	default:
		break;
	}
}

void TranslatingModel()
{
	if (g_TranslationVelocity.LengthSquared() > 0.009)
	{
		g_Models[g_CurrentModelIdx].Position += g_TranslationVelocity;
		glutPostRedisplay();
	}
}

int main(int argc, char **argv)
{
	cout << "Loading assets..." << endl;
	auto hr = LoadAssetes();
	if (!hr)
	{
		cout << "Error : Failed to loading assetes.";
		return hr;
	}
	cout << "Assets loaded." << endl;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(640, 480);
	g_CurrentGLUTWindow = glutCreateWindow("ObjViewer");

	// GLUT event call backs
	glutReshapeFunc(OnWindowResizing);
	glutDisplayFunc(OnSceneRedrawing);
	glutMouseFunc(OnMouseButtonStateChanged);
	glutMotionFunc(OnMouseMotion_TrackingRotation);
	glutKeyboardFunc(OnKeyPressed);
	glutKeyboardUpFunc(OnKeyReleased);
	glutIdleFunc(TranslatingModel);
	glutSpecialFunc(OnSpecialKeyPressed);
	glutSpecialUpFunc(OnSpecialKeyReleased);

	glEnable(GL_DEPTH_TEST);
	SetOrthoProjection();

	glutMainLoop();
}
