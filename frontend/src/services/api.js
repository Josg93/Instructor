import axios from 'axios';

const api = axios.create({
  baseURL: 'http://localhost:5555',
  headers: {
    'Content-Type' : 'application/json',
    'Accept' : 'application/json'
  },
  withCredentials: true
});

// Interceptor para añadir token
api.interceptors.request.use((config) => {
  const token = localStorage.getItem('token');
  if (token) {
    config.headers.Authorization = `Bearer ${token}`;
  }
  return config;
});

export const login = async (nombre, password) => {
  const response = await api.post('/auth/login', { nombre, password });
  return response.data;
};

export const register = async (nombre, password, tipo) => {
  const response = await api.post('/auth/registro', { nombre, password, tipo });
  return response.data;
};

export const getStudentCourses = async () => {
  const response = await api.get('/estudiante/cursos');
  return response.data;
};

export const enrollInCourse = async (courseId) => {
  const response = await api.post('/estudiante/cursos', { curso_id: courseId });
  return response.data;
};

export const getTeacherCourses = async () => {
  const response = await api.get('/profesor/cursos');
  return response.data;
};

export const createCourse = async (nombre) => {
  const response = await api.post('/profesor/cursos', { nombre });
  return response.data;
};