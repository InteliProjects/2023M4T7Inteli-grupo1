import React, { useEffect, useState } from 'react';

const Insights = () => {
  const [pagerData, setPagerData] = useState([]);
  const [emergenciesData, setEmergenciesData] = useState([]);

  useEffect(() => {
    fetch('http://localhost:8081/get_pager_data')
      .then((response) => response.json())
      .then((data) => setPagerData(data))
      .catch((error) => console.error('Erro ao buscar dados do pager:', error));
  }, []);

  useEffect(() => {
    fetch('http://localhost:8081/get_emergencies_data')
      .then((response) => response.json())
      .then((data) => setEmergenciesData(data))
      .catch((error) => console.error('Erro ao buscar dados das emergências:', error));
  }, []);

  const countDevicesInRooms = () => {
    const countMap = {};
    pagerData.forEach((pager) => {
      const room = pager.local;
      countMap[room] = (countMap[room] || 0) + 1;
    });
    return countMap;
  };

  const devicesInRooms = countDevicesInRooms();

  // Filter
  const openEmergencies = emergenciesData.filter((emergency) => emergency.status === 'aberta');
  console.log(openEmergencies)

  return (
    <div className="ml-4 bg-white rounded-lg p-4">
      <h2 className="text-xl text-blue-700 font-bold mb-4">Dispositivos por sala:</h2>
      <table className="table-auto mx-auto">
        <thead>
          <tr>
            <th className="border px-4 py-2 text-blue-800 font-bold">Sala</th>
            <th className="border px-4 py-2 text-blue-800 font-bold">Quantidade de Dispositivos</th>
          </tr>
        </thead>
        <tbody>
          {Object.entries(devicesInRooms).map(([room, count]) => (
            <tr key={room} className="text-blue-900">
              <td className="border px-4 py-2">{room}</td>
              <td className="border px-4 py-2">{count}</td>
            </tr>
          ))}
        </tbody>
      </table>
      <br></br>
      <h2 className="text-xl text-blue-700 font-bold mb-2">Emergências abertas:</h2>
      <table className="table-auto">
        <thead>
          <tr>
            <th className="border px-4 py-2 text-blue-800 font-bold">Emergência</th>
            <th className="border px-4 py-2 text-blue-800 font-bold">Local</th>
            <th className="border px-4 py-2 text-blue-800 font-bold">Severidade</th>
            <th className="border px-4 py-2 text-blue-800 font-bold">Data de Criação</th>
          </tr>
        </thead>
        <tbody>
          {openEmergencies.map((emergency) => (
            <tr key={emergency.id} className="text-blue-900">
              <td className="border px-4 py-2">{emergency.emerg}</td>
              <td className="border px-4 py-2">{emergency.local}</td>
              <td className="border px-4 py-2">
                {emergency.level === 1 && 'Baixa'}
                {emergency.level === 2 && 'Média'}
                {emergency.level === 3 && 'Alta'}
              </td>
              <td className="border px-4 py-2">
                {new Date(emergency.creation_time).toLocaleString()}
              </td>
            </tr>
          ))}
        </tbody>
      </table>
    </div>
  );
};

export default Insights;