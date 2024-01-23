import React, { useEffect, useState } from 'react';
import Chart from 'react-apexcharts';

const PagerChart = () => {
  const [pagerData, setPagerData] = useState([]);

  useEffect(() => {
    fetch('http://localhost:8081/get_pager_data')
      .then((response) => response.json())
      .then((data) => setPagerData(data))
      .catch((error) => console.error('Erro ao buscar dados do pager:', error));
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

  // Dados para o gráfico
  const chartData = {
    options: {
      chart: {
        id: 'devices-chart',
        toolbar: {
          show: false, // Oculta a barra de ferramentas do gráfico
        },
      },
      xaxis: {
        categories: Object.keys(devicesInRooms),
      },
    },
    series: [
      {
        name: 'Quantidade de Dispositivos por Sala',
        data: Object.values(devicesInRooms),
      },
    ],
  };

  return (
    <div className="flex flex-col items-center ml-4 bg-white rounded-lg p-4 mb-8">
      <h2 className="text-xl text-blue-700 font-bold mb-4">Dispositivos por sala (Gráfico):</h2>
      <Chart
        options={chartData.options}
        series={chartData.series}
        type="bar"
        width="500"
      />
    </div>
  );
};

export default PagerChart;
