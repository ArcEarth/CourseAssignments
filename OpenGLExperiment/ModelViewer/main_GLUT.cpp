#include <fstream>
#include <iostream>

#define GLEW_STATIC
#include <gl\glew.h>
#include <gl\glut.h>

#include <DirectXMath.h>
#include "SimpleMath.h"

#include "ObjModel.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace Models;

int		winWidth, winHeight;
int		g_CurrentGLUTWindow;

bool	g_TrackingMouse = false;
bool	g_Translating = false;

//std::string TerrainKey = "Terrain";
//std::vector<Models::RigidObjModel> g_Models;	// The models we load from obj files.
std::unique_ptr<RigidModel>			g_pTerrain;
std::map<std::string, std::unique_ptr<RigidModel>>	g_Objects;

GLuint		g_TextureEffect;
GLuint		g_PureColorEffect;

size_t		g_CurrentModelIdx;						// Obsolate global
Camera		g_Camera;
Vector3		g_RotationStartPoint;					// Record the last mouse postion for impleament the rotation operation
Vector3		g_TranslationVelocity;
Vector2		g_CameraPatchYaw;

Color		g_MateriaColor;
Color		g_AmbientLightColor;
Color		g_DirectionalLightColor;
Color		g_SpotLightColor_1;
Color		g_FogColor;								// Special : w/A component represent the visiable factor
Vector3		g_DirectionalLightDirection;
Vector3		g_SpotLightPosition_1;
Vector4		g_SpotLightDirection_1;

GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);


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
	if (v.x > 1.0) v.x = 1.0f;
	if (v.x < -1.0f) v.x = -1.0f;
	if (v.y > 1.0) v.y = 1.0f;
	if (v.y < -1.0f) v.y = -1.0f;
	if (v.LengthSquared() > 1.0f)
	{
		v.z = 0.0f;
		v.Normalize();
	}
	else
	{
		v.z = -sqrt(1.0f - v.LengthSquared());
	}
	//v.Normalize();
	return v;
}

Vector2 ConvertToPolarCoord(Vector3 pos)
{
	pos.Normalize();
	float r = sqrtf(pos.x*pos.x + pos.z * pos.z);
	float theta = atanf(pos.y / r);
	float phi = acosf(pos.x / r);
	return Vector2(theta, phi);
}

void OnMouseMotion_TrackingRotation(int x, int y)
{
	auto endSpherePoint = ProjectScreenIntoUintSphere(XMUINT2( x, y ), winWidth, winHeight);
	auto sp = ConvertToPolarCoord(g_RotationStartPoint);
	auto ep = ConvertToPolarCoord(endSpherePoint);

	auto delta = (ep - sp);

	if (delta.LengthSquared() > 0.1f)
	{
		g_RotationStartPoint = endSpherePoint;
		return;
	}

	g_CameraPatchYaw += delta;
	g_RotationStartPoint = endSpherePoint;

	if (delta.LengthSquared() > 0.00001f)
	{
		g_Camera.Orientation = XMQuaternionRotationRollPitchYaw(g_CameraPatchYaw.x, g_CameraPatchYaw.y, 0);
		//XMQuaternionMultiply(XMQuaternionRotationRollPitchYaw(delta.x, delta.y, 0), g_Camera.Orientation);
		glutPostRedisplay();
	}

	//float angle = delta.Length();
	//auto axis = g_RotationStartPoint.Cross(endSpherePoint);
	//if (axis.LengthSquared() > 0.001f)
	//{
	//	//g_Models[g_CurrentModelIdx].Orientation *= XMQuaternionRotationAxis(axis, angle);
	//	g_Camera.Orientation *= XMQuaternionRotationAxis(axis, angle);
	//	g_RotationStartPoint = endSpherePoint;
	//}

}

void RotationTracking_Begin(int x, int y)
{
	g_TrackingMouse = true;
	g_RotationStartPoint = ProjectScreenIntoUintSphere(XMUINT2( x, y ), winWidth, winHeight);
}
void RotationTracking_End(int x, int y)
{
	g_TrackingMouse = false;
	//auto endSpherePoint = ProjectScreenIntoUintSphere(XMUINT2(x, y), winWidth, winHeight);
	//auto sp = ConvertToPolarCoord(g_RotationStartPoint);
	//auto ep = ConvertToPolarCoord(endSpherePoint);
	//auto delta = (ep - sp);
	//g_CameraPatchYaw += delta;
	//g_Camera.Orientation = XMQuaternionRotationRollPitchYaw(g_CameraPatchYaw.x, g_CameraPatchYaw.y, 0);
	//glutPostRedisplay();
}

GLuint CreateMeshBuffer(const Models::RigidObjModel& model)
{
	return 0;
}

GLuint g_VerticesBuffer, g_ColorBuffer, g_TexCoordBuffer;
std::shared_ptr<GL::Texture2D> g_TerrianTexture;

void RenderTexQuad()
{

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0, 0,-2);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0, 1.0,-2);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(1, 1,-2);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(1, 0,-2);
	glEnd();
}

void RenderRigidModel(const Models::RigidModel& model)
{
	glPushMatrix();
	// Setup the model's rigid transformation
	Matrix mat = model.GetModelMatrix();
	glMultMatrixf((float*) &mat);

	mat = g_Camera.GetViewMatrix();
	glMultMatrixf((float*) &mat);

	const auto& vertices = dynamic_cast<const BasicVertexCollection&>(model.Mesh()->Vertices());
	const auto& facts = model.Mesh()->Facets();
	const uint32_t* indices = &facts[0].x;

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vertices.GetVertexBuffer());
	glVertexPointer(3, GL_FLOAT, 3 * sizeof(GLfloat), 0);

	if (vertices.HasColor())
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, vertices.GetColorBuffer());
		glColorPointer(4, GL_FLOAT, 4 * sizeof(GLfloat), 0);
	}

	if (vertices.HasTexCoord())
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, vertices.GetTexCoordBuffer());
		glTexCoordPointer(2, GL_FLOAT, 2 * sizeof(GLfloat), 0);

		if (model.HasColorTexture())
		glBindTexture(GL_TEXTURE_2D, *model.ColorTexture());
	}

	if (vertices.HasNormal())
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, vertices.GetNormalBuffer());
		glTexCoordPointer(3, GL_FLOAT, 3 * sizeof(GLfloat), 0);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.Mesh()->GetElementBuffer());
	glDrawElements(GL_TRIANGLES, facts.size() * 3, GL_UNSIGNED_INT, 0);
	//glDrawElements(GL_TRIANGLES, facts.size() * 3, GL_UNSIGNED_INT, indices);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	glPopMatrix();
}

void RenderRigidModelWithEffect(const Models::RigidModel& model)
{
	using namespace DirectX;
	const auto& vertices = dynamic_cast<const BasicVertexCollection&>(model.Mesh()->Vertices());
	const auto& facts = model.Mesh()->Facets();
	const uint32_t* indices = &facts[0].x;

	g_SpotLightDirection_1 = g_Camera.GetForward();
	g_SpotLightDirection_1.w = 0.9f;

	GLuint effect;
	if (model.HasColorTexture())
	{
		effect = g_TextureEffect;
	}
	else
	{
		effect = g_PureColorEffect;
	}


	glUseProgram(effect);

	// Common party
	// World-View-Projection Matrix
	XMMATRIX world, view, projection;

	GLuint MatrixID = glGetUniformLocation(effect, "ModelMatrix");
	Matrix mat = world = model.GetModelMatrix();
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, (float*) &mat);

	mat = view = g_Camera.GetViewMatrix();
	MatrixID = glGetUniformLocation(effect, "ViewMatrix");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, (float*) &mat);

	projection = g_Camera.GetProjectMatrix();
	//mat = projection;
	//MatrixID = glGetUniformLocation(effect, "ProjectionMatrix");
	//glUniformMatrix4fv(MatrixID, 1, GL_FALSE, (float*) &mat);

	mat = world * view * projection;
	MatrixID = glGetUniformLocation(effect, "ModelViewProjection");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, (float*) &mat);

	mat = world * view;
	MatrixID = glGetUniformLocation(effect, "ModelViewMatrix");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, (float*) &mat);

	// Light Colors
	MatrixID = glGetUniformLocation(effect, "AmbientLightColor");
	glUniform4f(MatrixID, g_AmbientLightColor.x, g_AmbientLightColor.y, g_AmbientLightColor.z, g_AmbientLightColor.w );
	MatrixID = glGetUniformLocation(effect, "DirectionalLightColor");
	glUniform4f(MatrixID, g_DirectionalLightColor.x, g_DirectionalLightColor.y, g_DirectionalLightColor.z, g_DirectionalLightColor.w);
	MatrixID = glGetUniformLocation(effect, "SpotLightColor_1");
	glUniform4f(MatrixID, g_SpotLightColor_1.x, g_SpotLightColor_1.y, g_SpotLightColor_1.z, g_SpotLightColor_1.w);

	// Light Position/Direction
	MatrixID = glGetUniformLocation(effect, "DirectionalLightDirection");
	glUniform3f(MatrixID, g_DirectionalLightDirection.x, g_DirectionalLightDirection.y, g_DirectionalLightDirection.z);
	MatrixID = glGetUniformLocation(effect, "SpotLightPosition_1");
	glUniform3f(MatrixID, g_SpotLightPosition_1.x, g_SpotLightPosition_1.y, g_SpotLightPosition_1.z);
	MatrixID = glGetUniformLocation(effect, "SpotLightDirection_1");
	glUniform4f(MatrixID, g_SpotLightDirection_1.x, g_SpotLightDirection_1.y, g_SpotLightDirection_1.z, g_SpotLightDirection_1.w);

	MatrixID = glGetUniformLocation(effect, "FogColor");
	glUniform4f(MatrixID, g_FogColor.x, g_FogColor.y, g_FogColor.z, g_FogColor.w);

	//uniform vec4 AmbientLightColor;
	//uniform vec4 DirectionalLightColor;
	//uniform vec3 DirectionalLightDirection;
	//uniform vec4 SpotLightColor_1;
	//uniform vec3 SpotLightPosition_1;

	// Position
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertices.GetVertexBuffer());
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);

	// Normal
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vertices.GetNormalBuffer());
	glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, 0);

	// Texture related
	if (model.HasColorTexture())
	{
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, vertices.GetTexCoordBuffer());
		glVertexAttribPointer(2, 2, GL_FLOAT, false, 0, 0);
		GLuint samplerID = glGetUniformLocation(effect, "ColorTextureSampler");
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, *model.ColorTexture());
		// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(samplerID, 0);
	}
	else
	{
		MatrixID = glGetUniformLocation(effect, "MaterialDiffuseColor");
		glUniform3f(MatrixID, g_MateriaColor.x, g_MateriaColor.y, g_MateriaColor.z);
	}

	// Indicis (Element) array
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.Mesh()->GetElementBuffer());
	glDrawElements(GL_TRIANGLES, facts.size() * 3, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	glUseProgram(0);
	//glPushMatrix();
	////glMatrixMode(GL_PROJECTION_MATRIX);
	//// Setup the model's rigid transformation
	//Matrix mat = model.GetModelMatrix();
	//glMultMatrixf((float*) &mat);
	//mat = g_Camera.GetViewMatrix();
	//glMultMatrixf((float*) &mat);

	//const auto& vertices = dynamic_cast<const BasicVertexCollection&>(model.Vertices());
	//glPopMatrix();
}

void RenderModelNormals(const Models::RigidModel& model)
{
	glPushMatrix();
	// Setup the model's rigid transformation
	Matrix mat = model.GetModelMatrix();
	glMultMatrixf((float*) &mat);

	mat = g_Camera.GetViewMatrix();
	//mat = mat.Transpose();
	glMultMatrixf((float*) &mat);

	mat = g_Camera.GetProjectMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((float*) &mat);
	glMatrixMode(GL_MODELVIEW);

	const auto& vertices = dynamic_cast<const BasicVertexCollection&>(model.Mesh()->Vertices());
	const auto& facets = model.Mesh()->Facets();
	const auto& mesh = *model.Mesh();
	const uint32_t* indices = &facets[0].x;
	Vector3 endPoint;
	glColor3f(1.0f, 0.4f, 0.4f);
	glBegin(GL_LINES);
	for (size_t i = 0; i < vertices.Count(); i++)
	{
		glVertex3fv(&vertices.Postions[i].x);
		endPoint = vertices.Postions[i] + vertices.Normals[i] * 0.2f;
		glVertex3fv(&endPoint.x);
	}
	glColor3f(0.4f, 0.4f, 1.0f);
	for (size_t i = 0; i < facets.size(); i++)
	{
		if (facets[i].x < facets[i].y)
		{
			glVertex3fv(&vertices.Postions[facets[i].x].x);
			glVertex3fv(&vertices.Postions[facets[i].y].x);
		}
		if (facets[i].y < facets[i].z)
		{
			glVertex3fv(&vertices.Postions[facets[i].y].x);
			glVertex3fv(&vertices.Postions[facets[i].z].x);
		}
		if (facets[i].z < facets[i].x)
		{
			glVertex3fv(&vertices.Postions[facets[i].z].x);
			glVertex3fv(&vertices.Postions[facets[i].x].x);
		}
	}


	glEnd();

	glPopMatrix();
}

bool RenderObjModelWithVertexIndexArray(const Models::RigidObjModel& model)
{
	glPushMatrix();
	// Setup the model's rigid transformation
	Matrix mat = model.GetModelMatrix();
	glMultMatrixf((float*) &mat);

	const auto& vertices = dynamic_cast<const BasicVertexCollection&>(model.Vertices());
	const auto& facts = model.Facets();
	const uint32_t* indices = &facts[0].x;

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, g_VerticesBuffer);
	glVertexPointer(3, GL_FLOAT, 3 * sizeof(GLfloat), 0);

	if (vertices.HasColor())
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, g_ColorBuffer);
		glColorPointer(4, GL_FLOAT, 4 * sizeof(GLfloat), 0);
	}

	if (vertices.HasTexCoord())
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, g_TexCoordBuffer);
		glTexCoordPointer(2, GL_FLOAT, 2 * sizeof(GLfloat), 0);

		glBindTexture(GL_TEXTURE_2D, *g_TerrianTexture);
	}

	if (vertices.HasNormal())
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, g_ColorBuffer);
		glTexCoordPointer(3, GL_FLOAT, 3 * sizeof(GLfloat), 0);
	}

	glDrawElements(GL_TRIANGLES, facts.size() * 3, GL_UNSIGNED_INT, indices);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	glPopMatrix();
	return true;
}

void OnSceneRedrawing(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	//glMultMatrixf((float*) &g_Camera.GetViewMatrix());

		//RenderTexQuad();
	//RenderRigidModel(*g_pTerrain);
	RenderRigidModelWithEffect(*g_pTerrain);
	//RenderModelNormals(*g_pTerrain);

	//RenderRigidModelWithEffect(*g_Objects["camel"]);
	for (const auto& obj : g_Objects)
	{
		RenderRigidModelWithEffect(*obj.second);
		RenderModelNormals(*obj.second);
	}
	//RenderObjModelWithVertexIndexArray(g_Models[g_CurrentModelIdx]);

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
	g_pTerrain = std::make_unique<RigidModel>("mesh\\puget_tex.obj", "texture\\puget_tex.ppm");
	//g_pTerrain = std::make_unique<RigidModel>("mesh\\gc_tex.obj", "texture\\gc_tex.ppm");

	g_Objects.emplace("camel", make_unique<RigidModel>("mesh\\camel.obj"));
	g_Objects.emplace("cow", make_unique<RigidModel>("mesh\\cow.obj"));
	g_Objects.emplace("enterprise", make_unique<RigidModel>("mesh\\enterprise.obj"));
	g_Objects.emplace("klingon", make_unique<RigidModel>("mesh\\klingon.obj"));
	g_Objects.emplace("rabbit", make_unique<RigidModel>("mesh\\rabbit.obj"));
	g_Objects.emplace("voyager", make_unique<RigidModel>("mesh\\voyager.obj"));

	Vector3 center(&g_pTerrain->Mesh()->MeshBoundingBox().Center.x);
	g_MateriaColor = Color(0.5f, 0.4f, 0.2f);
	g_Objects["camel"]->Position = center + Vector3(0.5f, 0.5f, -2.0f);
	g_Objects["cow"]->Position = center + Vector3(2.0f, 0.5f, -2.0f);
	g_Objects["enterprise"]->Position = center + Vector3(3.0f, 0.5f, -2.0f);
	g_Objects["rabbit"]->Position = center + Vector3(4.0f, 0.5f, -2.0f);
	g_Objects["klingon"]->Position = center + Vector3(5.0f, 0.5f, -2.0f);
	g_Objects["voyager"]->Position = center + Vector3(6.0f, 0.5f, -2.0f);
	return true;
}

void SetupCameraAndLights()
{
	g_Camera.Position = XMLoadFloat3(&g_pTerrain->Mesh()->MeshBoundingBox().Center);
	g_Camera.Aspect = (float) winWidth / (float) winHeight;
	g_Camera.Position.y += 1.0f;
	//g_Camera.LookTo(Vector3(0.0f, 0.0f, -1.0f));

	g_AmbientLightColor = Color(0.2f,0.2f,0.2f,1.0f);
	//g_DirectionalLightColor = Color(0.0f, 0.0f, 0.0f);
	g_DirectionalLightColor = Color(0.4f, 0.4f, 0.4f, 1.0f);
	g_SpotLightColor_1 = Color(0.6f, 0.6f, 0.6f, 20.0f);
	(g_DirectionalLightDirection = Vector3(1.0f,-1.0f,-1.0f)).Normalize();

	g_SpotLightPosition_1 = g_Camera.Position;
	g_FogColor = Color(1.0f, 1.0f, 1.0f, 0.005);
}

void OnKeyPressed(unsigned char key, int x, int y)
{
	switch (key){

		// Continues keystrokes
	case 'w':
	{
		g_TranslationVelocity.z = -0.1f;
		break;
	}
	case 's':
	{
		g_TranslationVelocity.z = 0.1f;
		break;
	}
	case 'a':
	{
		g_TranslationVelocity.x = -0.1f;
		break;
	}
	case 'd':
	{
		g_TranslationVelocity.x = 0.1f;
		break;
	}
	case 'z':
	{
		g_TranslationVelocity.y = 0.1f;
		break;
	}
	case 'x':
	{
		g_TranslationVelocity.y = -0.1f;
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
		case 'w':
		case 's':
		{
			g_TranslationVelocity.z = 0.0f;
			break;
		}
		case 'a':
		case 'd':
		{
			g_TranslationVelocity.x = 0.0f;
			break;
		}
		case 'z':
		case 'x':
		{
			g_TranslationVelocity.y = 0.0f;
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

void TranslatingCamera()
{
	if (g_TranslationVelocity.LengthSquared() > 0.009)
	{
		g_Camera.Move(g_TranslationVelocity);
		g_SpotLightPosition_1 = g_Camera.Position;
		//g_Models[g_CurrentModelIdx].Position += g_TranslationVelocity;
		glutPostRedisplay();
	}
}

int main(int argc, char **argv)
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	winWidth = 640; winHeight = 480;
	glutInitWindowSize(winWidth, winHeight);
	g_CurrentGLUTWindow = glutCreateWindow("ObjViewer");
	//glutSetCursor(GLUT_CURSOR_NONE);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// GLUT event call backs
	glutReshapeFunc(OnWindowResizing);
	glutDisplayFunc(OnSceneRedrawing);
	glutMouseFunc(OnMouseButtonStateChanged);
	glutMotionFunc(OnMouseMotion_TrackingRotation);
	glutKeyboardFunc(OnKeyPressed);
	glutKeyboardUpFunc(OnKeyReleased);
	glutIdleFunc(TranslatingCamera);
	glutSpecialFunc(OnSpecialKeyPressed);
	glutSpecialUpFunc(OnSpecialKeyReleased);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	cout << "Loading assets..." << endl;
	auto hr = LoadAssetes();
	if (!hr)
	{
		cout << "Error : Failed to loading assetes.";
		return hr;
	}
	cout << "Assets loaded." << endl;

	cout << "Loading shaders...";
	g_TextureEffect = LoadShaders("Texture_VertexLighting.vs.glsl",
		"Texture_VertexLighting.fs.glsl");
	g_PureColorEffect = LoadShaders("VertexColorLighting.vs.glsl",
		"VertexColorLighting.fs.glsl");
	cout << "Setting up enviruments...";
	SetupCameraAndLights();

	//CreateBuffers(g_Models[g_CurrentModelIdx]);

	glutMainLoop();
}

GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path){

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open())
	{
		std::string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()){
		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> VertexShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);

	// Link the program
	fprintf(stdout, "Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> ProgramErrorMessage(max(InfoLogLength, int(1)));
	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}
