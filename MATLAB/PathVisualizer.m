%% Import Data
clear all ;
clc;
close all ;

% Import traces
data1  = readtable('../logging/p000_tFITTS_r1.txt', 'Delimiter', ',', 'ReadVariableNames', true, 'VariableNamingRule','preserve');
disp(data1.Properties.VariableNames);

t = data1.timestamp ;
x = data1.errZmm ;
y = data1.errYmm ;
z = data1.errXmm*-1 ;


% Normalize timestamps to [0, 1] for color mapping
tNorm = (t - min(t)) / (max(t) - min(t));

% Create colormap from red to blue
cmap = [linspace(1,0,length(t))', zeros(length(t),1), linspace(0,1,length(t))'];  % RGB: red→blue



% Create figure
figure;
hold on;
label = text ( NaN , NaN , NaN , '') ;
for i = 1:length(x)-1
    plot3(x(i:i+1), y(i:i+1), z(i:i+1), '-', 'Color', cmap(i,:), 'LineWidth', 2);
    set(label, 'Position', [0,0,0],'String', sprintf('t=%.2f', t(i))) ;

end
xlabel('Z [mm]');
ylabel('X [mm]');
zlabel('Y [mm]');
title('Trajectory');
view(45, 30);   % Azimuth = 45°, Elevation = 30°
grid on;

% Set axis ticks (optional)
 xlim([0, 500]);
 ylim([-200, 200]);
zlim([-200, 200]);
% xticks(-100:50:100);        % Axis tick spacing
% yticks(-100:50:100);        % Axis tick spacing
% zticks(-200:50:700);        % Axis tick spacing
hold off; 


% % Create a 3D animated trajectory
% figure;
% 
% % Plot
% %comet3(x, y, z);
% plot3(x,y,z, 'o-') ; 
% 
% 
% % Set axis limits
% xlim([-100, 100]);
% ylim([-100, 100]);
% zlim([0, 700]);
% xticks(-100:50:100);        % Axis tick spacing
% yticks(-100:50:100);        % Axis tick spacing
% zticks(-200:50:700);        % Axis tick spacing
% 
% 
% 
% % Set axis labels
% xlabel('X [mm]');  % Horizontal
% ylabel('Y [mm]');  % Vertical
% zlabel('Z [mm]');  % Depth into screen
% 
% 
% % Set view to make Z axis go into screen
% % view([0, 0, 1]);  % Camera looking along positive Z
% grid on;
% 
% title('3D Dot Trajectory');