import React, { useEffect, useState } from 'react';
import Insights from './components/Insights';
import EmergencyCallForm from './components/EmergencyCallForm';
import PagerChart from './components/PagerChart';

const HospitalMap = () => {
  const [pagerData, setPagerData] = useState([]);

  useEffect(() => {
    fetch('http://localhost:8081/get_pager_data')
      .then((response) => response.json())
      .then((data) => setPagerData(data))
      .catch((error) => console.error('Erro ao buscar dados do pager:', error));
  }, []);

  useEffect(() => {
    const canvas = document.getElementById('mapCanvas');
    const ctx = canvas.getContext('2d');

    const getPagersCountByRoom = (room) => {
      return pagerData.filter((pager) => pager.local === room).length;
    };

    drawRoom(ctx, 80, 130, 80, 80, 'Sala 01', getPagersCountByRoom('Sala 01'));
    drawRoom(ctx, 80, 50, 80, 80, 'Sala 02', getPagersCountByRoom('Sala 02'));
    drawRoom(ctx, 210, 50, 120, 120, 'Sala 03', getPagersCountByRoom('Sala 03'));
    drawRoom(ctx, 210, 170, 120, 120, 'Sala 04', getPagersCountByRoom('Sala 04'));
  }, [pagerData]);

  const drawRoom = (ctx, x, y, width, height, label, pagersCount) => {
    let fillColor;
    if (pagersCount === 0) {
      fillColor = '#F2F2F2';
    } else if (pagersCount === 1) {
      fillColor = '#79F2C6';
    } else if (pagersCount === 2 || pagersCount === 3) {
      fillColor = '#F2CF79';
    } else if (pagersCount >= 4) {
      fillColor = '#F2B279';
    }
    
    // Desenha o retângulo da sala
    ctx.fillStyle = fillColor; // Cor do preenchimento
    ctx.fillRect(x, y, width, height);

    // Adiciona a borda preta para as salas
    ctx.strokeStyle = '#000000';
    ctx.lineWidth = 1;
    ctx.strokeRect(x, y, width, height);

    // Adiciona a etiqueta da sala
    ctx.fillStyle = '#000'; // Cor do texto
    ctx.font = '14px Arial';
    ctx.textAlign = 'center';
    ctx.textBaseline = 'middle';
    ctx.fillText(label, x + width / 2, y + height / 2);
  };

  return (
    <canvas
      id="mapCanvas"
      width="400"
      height="400"
      className="border border-black mx-auto bg-white rounded-2xl"
    ></canvas>
  );
};

const App = () => {
  const handleEmergencySubmit = async (data) => {
    console.log('Dados do chamado de emergência enviados com sucesso:', data);
  };

  return (
    <div className="bg-gradient-to-br from-blue-400 via-blue-500 to-blue-800 min-h-screen flex items-center justify-center">
      <div className="text-center text-white">
        <h1 className="text-4xl font-bold font-sans mb-8 mt-4">Delphlocator</h1>
        <div className="flex">
          <HospitalMap />
          <Insights />
        </div>
        <EmergencyCallForm onSubmit={handleEmergencySubmit} />
        <PagerChart />
      </div>
    </div>
  );
};

export default App;