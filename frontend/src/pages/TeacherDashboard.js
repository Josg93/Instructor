import React, { useState, useEffect } from 'react';
import { getTeacherCourses, createCourse } from '../services/api';

function TeacherDashboard({ user, onLogout }) {
  const [courses, setCourses] = useState([]);
  const [newCourseName, setNewCourseName] = useState('');

  useEffect(() => {
    const fetchCourses = async () => {
      const data = await getTeacherCourses();
      setCourses(data);
    };
    fetchCourses();
  }, []);

  const handleCreateCourse = async (e) => {
    e.preventDefault();
    const newCourse = await createCourse(newCourseName);
    setCourses([...courses, newCourse]);
    setNewCourseName('');
  };

  return (
    <div className="dashboard">
      <h2>Bienvenido, Profesor {user.nombre}</h2>
      <button onClick={onLogout}>Cerrar sesión</button>

      <h3>Tus cursos:</h3>
      <ul>
        {courses.map(course => (
          <li key={course.id}>{course.nombre}</li>
        ))}
      </ul>

      <h3>Crear nuevo curso:</h3>
      <form onSubmit={handleCreateCourse}>
        <input
          type="text"
          placeholder="Nombre del curso"
          value={newCourseName}
          onChange={(e) => setNewCourseName(e.target.value)}
          required
        />
        <button type="submit">Crear</button>
      </form>
    </div>
  );
}

export default TeacherDashboard;