import React, { useState } from 'react';
import Login from './pages/Login';
import Register from './pages/Register';
import StudentDashboard from './pages/StudentDashboard';
import TeacherDashboard from './pages/TeacherDashboard';
import "./App.css"

function App() {
  const [user, setUser] = useState(null);
  const [page, setPage] = useState('login');

  const handleLogin = (userData) => {
    setUser(userData);
    setPage(userData.tipo === 'profesor' ? 'teacher' : 'student');
  };

  const handleLogout = () => {
    setUser(null);
    setPage('login');
    localStorage.removeItem('token');
  };

  return (
    <div className="app">
      {page === 'login' && <Login onLogin={handleLogin} goToRegister={() => setPage('register')} />}
      {page === 'register' && <Register goToLogin={() => setPage('login')} />}
      {page === 'student' && <StudentDashboard user={user} onLogout={handleLogout} />}
      {page === 'teacher' && <TeacherDashboard user={user} onLogout={handleLogout} />}
    </div>
  );
}

export default App;