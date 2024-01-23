import React, { useState } from 'react';

const EmergencyCallForm = ({ onSubmit }) => {
  const [emergency, setEmergency] = useState('');
  const [location, setLocation] = useState('Sala 01'); // padrão
  const [severity, setSeverity] = useState(1); // padrão
  const [showFeedback, setShowFeedback] = useState(false);
  const [feedbackMessage, setFeedbackMessage] = useState('');

  const handleSubmit = (e) => {
    e.preventDefault();

    const sendEmergencyData = async () => {
        try {
            const res = await fetch('http://localhost:8081/create_emergency', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ emergency, location, severity }),
            });
    
            const data = await res.json();
            console.log('Backend réplica >:( ', data);
    
            onSubmit(data);

            setShowFeedback(true);
            setFeedbackMessage('Emergência criada com sucesso!');
    
            // Aqui limpam-se os dados após o envio das informações
            setEmergency('');
            setLocation('Sala 01');
            setSeverity(1);
        } catch (error) {
            console.error('Erro ao criar o chamado de emergência via web :( ', error.message);
        }
    };

    sendEmergencyData();
  };

  return (
    <div className="bg-white p-4 rounded-lg shadow-md mt-4 mb-8">
      <h2 className="text-xl font-semibold mb-4">Chamado de Emergência</h2>
      <form onSubmit={handleSubmit}>
        <div className="mb-4">
          <label htmlFor="emergency" className="block text-gray-700 font-bold mb-2">
            Tipo de Emergência:
          </label>
          <input
            type="text"
            id="emergency"
            className="border rounded w-full py-2 px-3 text-black"
            placeholder="Escreva brevemente a emergência"
            value={emergency}
            onChange={(e) => setEmergency(e.target.value)}
          />
        </div>

        <div className="mb-4">
          <label htmlFor="location" className="block text-gray-700 font-bold mb-2">
            Local:
          </label>
          <select
            id="location"
            className="border rounded w-full py-2 px-3 text-black"
            value={location}
            onChange={(e) => setLocation(e.target.value)}
          >
            <option value="Sala 01">Sala 01</option>
            <option value="Sala 02">Sala 02</option>
            <option value="Sala 03">Sala 03</option>
            <option value="Sala 04">Sala 04</option>
          </select>
        </div>

        <div className="mb-4">
          <label htmlFor="severity" className="block text-gray-700 font-bold mb-2">
            Severidade:
          </label>
          <select
            id="severity"
            className="border rounded w-full py-2 px-3 text-black"
            value={severity}
            onChange={(e) => setSeverity(parseInt(e.target.value))}
          >
            <option value={1}>Baixa</option>
            <option value={2}>Média</option>
            <option value={3}>Alta</option>
          </select>
        </div>

        <button
          type="submit"
          className="bg-gradient-to-br from-blue-500 to-blue-600 text-white py-2 px-4 rounded focus:outline-none focus:shadow-outline-blue"
        >
          Chamar Equipe Médica
        </button>

        {showFeedback && (
          <div className="mt-2 text-green-600">
            {feedbackMessage}
          </div>
        )}
      </form>
    </div>
  );
};

export default EmergencyCallForm;
