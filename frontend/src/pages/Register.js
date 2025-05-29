import React, { useState } from 'react';
import { register } from '../services/api';

function Register({ goToLogin }) {
  const [nombre, setNombre] = useState('');
  const [password, setPassword] = useState('');
  const [tipo, setTipo] = useState('estudiante');
  const [error, setError] = useState('');

  const handleSubmit = async (e) => {
    e.preventDefault();
    try {
      await register(nombre, password, tipo);
      goToLogin();
    } catch (err) {
      setError('Error al registrar');
    }
  };

  return (
    <div className="register-form">
      <h2>Registro</h2>
      {error && <p className="error">{error}</p>}
      <form onSubmit={handleSubmit}>
        <input
          type="text"
          placeholder="Nombre de usuario"
          value={nombre}
          onChange={(e) => setNombre(e.target.value)}
          required
        />
        <input
          type="password"
          placeholder="Contraseña"
          value={password}
          onChange={(e) => setPassword(e.target.value)}
          required
        />
        <select value={tipo} onChange={(e) => setTipo(e.target.value)}>
          <option value="estudiante">Estudiante</option>
          <option value="profesor">Profesor</option>
        </select>
        <button type="submit">Registrarse</button>
      </form>
      <p>
        ¿Ya tienes cuenta? <button onClick={goToLogin}>Inicia sesión aquí</button>
      </p>
    </div>
  );
}

export default Register;