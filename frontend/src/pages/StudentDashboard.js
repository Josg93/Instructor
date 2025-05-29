import React, { useState, useEffect } from 'react';
import { getStudentCourses, enrollInCourse } from '../services/api';

function StudentDashboard({ user, onLogout }) {
  const [courses, setCourses] = useState([]);
  const [availableCourses, setAvailableCourses] = useState([]);

  useEffect(() => {
    const fetchCourses = async () => {
      const enrolled = await getStudentCourses();
      setCourses(enrolled);
      // Simulamos cursos disponibles (en un caso real harías otra petición)
      setAvailableCourses([{ id: 1, nombre: 'Matemáticas' }, { id: 2, nombre: 'Historia' }]);
    };
    fetchCourses();
  }, []);

  const handleEnroll = async (courseId) => {
    await enrollInCourse(courseId);
    const enrolled = await getStudentCourses();
    setCourses(enrolled);
  };

  return (
    <div className="dashboard">
      <h2>Bienvenido, {user.nombre}</h2>
      <button onClick={onLogout}>Cerrar sesión</button>

      <h3>Tus cursos:</h3>
      <ul>
        {courses.map(course => (
          <li key={course.id}>{course.nombre}</li>
        ))}
      </ul>

      <h3>Cursos disponibles:</h3>
      <ul>
        {availableCourses.map(course => (
          <li key={course.id}>
            {course.nombre}
            <button onClick={() => handleEnroll(course.id)}>Inscribirse</button>
          </li>
        ))}
      </ul>
    </div>
  );
}

export default StudentDashboard;