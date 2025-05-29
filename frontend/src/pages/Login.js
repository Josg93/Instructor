import React, { useState } from 'react';
import { login } from '../services/api';

function Login({ onLogin, goToRegister }) {
  const [nombre, setNombre] = useState('');
  const [password, setPassword] = useState('');
  const [error, setError] = useState('');

  const handleSubmit = async (e) => {
    e.preventDefault();
    try {
      const { token } = await login(nombre, password);
      localStorage.setItem('token', token);
      const user = { nombre, tipo: token.tipo }; // Simplificado
      onLogin(user);
    } catch (err) {
      setError('Credenciales incorrectas');
    }
  };

  return (
    <div className="login-form">
      <h2>Iniciar Sesión</h2>
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
        <button type="submit">Ingresar</button>
      </form>
      <p>
        ¿No tienes cuenta? <button onClick={goToRegister}>Regístrate aquí</button>
      </p>
    </div>
  );
}

export default Login;