
#include <Python.h>
#include <cmath>

typedef double vector3[3];

#define EPSILON 1e-8
#define Deg2Rad 0.0174532925

#define vector_sub(x, y) {x[0]-y[0], x[1]-y[1], x[2]-y[2]}
#define vector_rotate_2d(_res, _vec3, theta)	\
	do {	\
		double s = std::sin(theta);	\
		double c = std::cos(theta);	\
		_res[0] = _vec3[2] * s + _vec3[0] * c;	\
		_res[2] = _vec3[2] * c - _vec3[0] * s;	\
	} while(0)

#define is_aabb_circle_intersect(cx, cy, w, l, px, py, r)	\
	do {	\
		double _x = std::abs(px - cx);	\
		double _y = std::abs(py - cy);	\
		_x = (_x < w) ? 0 : _x - w;	\
		_y = (_y < l) ? 0 : _y - l;	\
		ret = (_x * _x + _y * _y <= r * r) ? 1 : 0;	\
	} while(0)

#define vector_pow2_length_2d(vec3) ((vec3[0] * vec3[0]) + (vec3[2] * vec3[2]))
#define vector_pow2_length_3d(vec3) ((vec3[0] * vec3[0]) + (vec3[1] * vec3[1]) + (vec3[2] * vec3[2]))
#define vector_length_2d(vec3) std::sqrt((vec3[0] * vec3[0]) + (vec3[2] * vec3[2]))
#define vector_length_3d(vec3) std::sqrt((vec3[0] * vec3[0]) + (vec3[1] * vec3[1]) + (vec3[2] * vec3[2]))

#define yaw_to_vector(yaw) {std::sin(yaw), 0, std::cos(yaw)}

#define normalize_sqr(vec3)			\
	do {		\
		double f = vector_length_3d(vec3);			\
		if(f < EPSILON) {							\
			vec3[0] = vec3[1] = vec3[2] = 0;		\
		}											\
		else {										\
			vec3[0] = vec3[0] / f;					\
			vec3[1] = vec3[1] / f;					\
			vec3[2] = vec3[2] / f;					\
		}											\
	} while (0)

#define dot_2d(v0, v1) (v0[0] * v1[0] + v0[2] * v1[2])

#define UTIL_PY_TO_VEC(vec3, py_vec)	\
	do {	\
		if (PyTuple_Check(py_vec)) {	\
			vec3[0] = PyFloat_AsDouble(PyTuple_GET_ITEM(py_vec, 0));		\
			vec3[1] = PyFloat_AsDouble(PyTuple_GET_ITEM(py_vec, 1));		\
			vec3[2] = PyFloat_AsDouble(PyTuple_GET_ITEM(py_vec, 2));		\
		}		\
		else if (PyList_Check(py_vec)) {	\
			vec3[0] = PyFloat_AsDouble(PyList_GET_ITEM(py_vec, 0));			\
			vec3[1] = PyFloat_AsDouble(PyList_GET_ITEM(py_vec, 1));			\
			vec3[2] = PyFloat_AsDouble(PyList_GET_ITEM(py_vec, 2));			\
		}		\
		else {	\
			Py_RETURN_FALSE;												\
		}		\
	} while(0)

static PyObject*
is_in_square3d(PyObject*, PyObject* args) {
	PyObject *py_center, *py_point;
	double yaw, width, length, up_height, down_height, radius;
	if(!PyArg_ParseTuple(
		args, "OdddddOd:is_in_square3d", 
		&py_center, &yaw, &width, &length, &up_height, &down_height, &py_point, &radius)) {
		return NULL;
	}
	vector3 center, point;
	UTIL_PY_TO_VEC(center, py_center);
	UTIL_PY_TO_VEC(point, py_point);
	if (point[1] - center[1] < down_height || point[1] - center[1] > up_height)
		Py_RETURN_FALSE;

	vector3 ab_p = vector_sub(point, center);
	vector3 rot;
	vector_rotate_2d(rot, ab_p, -yaw);
	int ret = 0;
	is_aabb_circle_intersect(0, length * 0.5, width * 0.5, length * 0.5, rot[0], rot[2], radius);
	if (ret) Py_RETURN_TRUE;
	else Py_RETURN_FALSE;
}

static PyObject*
is_in_cylinder(PyObject*, PyObject* args) {
	PyObject *py_center, *py_point;
	double radius, up_height, down_height, t_radius;
	if(!PyArg_ParseTuple(
		args, "OdddOd:is_in_cylinder",
		&py_center, &radius, &up_height, &down_height, &py_point, &t_radius)) {
		return NULL;
	}
	vector3 center, point;
	UTIL_PY_TO_VEC(center, py_center);
	UTIL_PY_TO_VEC(point, py_point);
	if (point[1] - center[1] < down_height || point[1] - center[1] > up_height)
		Py_RETURN_FALSE;
	vector3 d = vector_sub(point, center);
	if (vector_pow2_length_2d(d) < (radius + t_radius) * (radius + t_radius)) Py_RETURN_TRUE;
	else Py_RETURN_FALSE;
}

static PyObject*
is_in_cylinder_fan(PyObject*, PyObject* args) {
	PyObject *py_center, *py_point;
	double radius, up_height, down_height, yaw, angle, t_radius;
	if(!PyArg_ParseTuple(
		args, "OdddddOd:is_in_cylinder_fan",
		&py_center, &radius, &up_height, &down_height, &yaw, &angle, &py_point, &t_radius)) {
		return NULL;
	}
	vector3 center, point;
	UTIL_PY_TO_VEC(center, py_center);
	UTIL_PY_TO_VEC(point, py_point);
	if (point[1] - center[1] < down_height || point[1] - center[1] > up_height)
		Py_RETURN_FALSE;
	vector3 d = vector_sub(point, center);
	if (vector_pow2_length_2d(d) > (radius + t_radius) * (radius + t_radius)) 
		Py_RETURN_FALSE;
	d[1] = 0;
	normalize_sqr(d);
	vector3 n = yaw_to_vector(yaw);
	if (dot_2d(d, n) > std::cos(angle * Deg2Rad)) Py_RETURN_TRUE;
	else Py_RETURN_FALSE;
}

static PyObject*
is_in_sphere(PyObject*, PyObject* args) {
	PyObject *py_center, *py_point;
	double radius, t_radius;
	if(!PyArg_ParseTuple(
		args, "OdOd:is_in_sphere",
		&py_center, &radius, &py_point, &t_radius)) {
		return NULL;
	}
	vector3 center, point;
	UTIL_PY_TO_VEC(center, py_center);
	UTIL_PY_TO_VEC(point, py_point);
	vector3 d = vector_sub(point, center);
	if (vector_pow2_length_3d(d) < (radius + t_radius) * (radius + t_radius))
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}

static PyObject*
is_in_circle(PyObject*, PyObject* args) {
	PyObject *py_center, *py_point;
	double radius, inner_radius, up_height, down_height, t_radius;
	if(!PyArg_ParseTuple(
		args, "OddddOd:is_in_circle",
		&py_center, &radius, &inner_radius, &up_height, &down_height, &py_point, &t_radius)) {
		return NULL;
	}
	vector3 center, point;
	UTIL_PY_TO_VEC(center, py_center);
	UTIL_PY_TO_VEC(point, py_point);
	if (point[1] - center[1] < down_height || point[1] - center[1] > up_height)
		Py_RETURN_FALSE;
	vector3 d = vector_sub(point, center);
	double dist = vector_pow2_length_2d(d);
	if (dist < (radius + t_radius) * (radius + t_radius) &&
		(inner_radius < t_radius || dist >(inner_radius - t_radius)*(inner_radius - t_radius)))
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}

static PyObject*
pow2_of_distance(PyObject*, PyObject* args) {
	PyObject *py_center, *py_point;
	if(!PyArg_ParseTuple(
		args, "OO:pow2_of_distance",
		&py_center, &py_point)) {
		return NULL;
	}
	vector3 center, point;
	UTIL_PY_TO_VEC(center, py_center);
	UTIL_PY_TO_VEC(point, py_point);
	vector3 d = vector_sub(point, center);
	double ret = vector_pow2_length_3d(d);
	return PyFloat_FromDouble(ret);
}

static PyObject*
pow2_of_distance2d(PyObject*, PyObject* args) {
	PyObject *py_center, *py_point;
	if(!PyArg_ParseTuple(
		args, "OO:pow2_of_distance2d",
		&py_center, &py_point)) {
		return NULL;
	}
	vector3 center, point;
	UTIL_PY_TO_VEC(center, py_center);
	UTIL_PY_TO_VEC(point, py_point);
	vector3 d = vector_sub(point, center);
	double ret = vector_pow2_length_2d(d);
	return PyFloat_FromDouble(ret);
}


static PyMethodDef formula_math_methods[] = {
	{"is_in_square3d", (PyCFunction)is_in_square3d, METH_VARARGS, ""},
	{"is_in_cylinder", (PyCFunction)is_in_cylinder, METH_VARARGS, "" },
	{"is_in_cylinder_fan", (PyCFunction)is_in_cylinder_fan, METH_VARARGS, "" },
	{"is_in_sphere", (PyCFunction)is_in_sphere, METH_VARARGS, "" },
	{"is_in_circle", (PyCFunction)is_in_circle, METH_VARARGS, "" },
	{"pow2_of_distance", (PyCFunction)pow2_of_distance, METH_VARARGS, "" },
	{"pow2_of_distance2d", (PyCFunction)pow2_of_distance2d, METH_VARARGS, "" },
	{NULL, NULL}
};

extern "C" {
#if defined(_WIN32)
__declspec(dllexport) 
#endif // _WIN32
#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef formula_math_mod =
{
	PyModuleDef_HEAD_INIT,
	"formula_math",				/* name of module */
	"formula C++ version.",		/* module documentation, may be NULL */
	-1,							/* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
	formula_math_methods	
};
void PyInit_formula_math() {
	PyModule_Create(&formula_math_mod);
}
#else
void initformula_math()
{
	Py_InitModule3("formula_math", formula_math_methods, "formula C++ version.");
}
#endif
}
