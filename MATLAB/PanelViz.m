%% Reset
clear all ;

%% Flags
ANIMATE = true ;

%% Helper items

% Colors
colRed = [ 1.00 , 0.00 , 0.00 ] ;
colRedLt = [ 1.00 , 0.60 , 0.60 ] ;
colRedDk = [ 0.60 , 0.00 , 0.00 ] ;
colOra = [ 0.90 , 0.39 , 0.00 ] ;
colYel = [ 0.90 , 0.90 , 0.00 ] ;
colGre = [ 0.20 , 0.80 , 0.20 ] ;
colGreLt = [ 0.60 , 0.90 , 0.60 ] ;
colGreDk = [ 0.0 , 0.60 , 0.00 ] ;
colBlu = [ 0.00 , 0.00 , 1.00 ] ;
colBluLt = [ 0.60 , 0.60 , 1.00 ] ;
colBluDk = [ 0.00 , 0.00 , 0.60 ] ;
colVio = [ 0.31 , 0.16 , 0.51 ] ;
colPin = [ 1.00 , 0.10 , 1.00 ] ;
colStart = [0.8 , 0.8 , 1] ;
colEnd = [ 0 , 0 , 1 ] ;
colGraLt = [ 0.8 , 0.8 , 0.8 ] ;

%% Import

clc;

% Import data
filename = 'FITTS_d0527_h202059' ;
telemetry = readtable([filename '.txt'], 'Delimiter', ',', 'ReadVariableNames', true, 'VariableNamingRule','preserve');
disp(telemetry.Properties.VariableNames);

% Load data
time = telemetry.timestamp(2:end);
y = telemetry.errXmm(2:end) ;
z = telemetry.errYmm(2:end) ;
x = telemetry.errZmm(2:end) ;

t1 = time(end) 
    t2 = length ( time ) 

%% Plot

clc;
close all ;

% Figure
figure ( 'Position', [10, 200, 1800, 800] ) ;


% Tiled layout
t = tiledlayout( 3 , 2 ) ;
t.TileSpacing = 'loose' ;
t.Padding = 'compact' ;

% Change tile
telemetryTile = nexttile ( t , [3 1] ) ; 
axes( telemetryTile ) ; 
hold on ;

% Initialize plot
telemetryPlot = plot3 ( telemetryTile , NaN , NaN , NaN , 'bo' , 'MarkerFaceColor', colStart, 'MarkerSize',6 ) ; 
telemetryShadowPlotY = plot3(telemetryTile , NaN, NaN , NaN , 'o', 'MarkerFaceColor', colGreLt, 'MarkerEdgeColor', 'none', 'MarkerSize', 2);
telemetryShadowPlotZ = plot3(telemetryTile , NaN, NaN , NaN , 'o', 'MarkerFaceColor', colGreLt, 'MarkerEdgeColor', 'none', 'MarkerSize', 2);
telemetryShadowPlotX = plot3(telemetryTile , NaN, NaN , NaN , 'o', 'MarkerFaceColor', colGreLt, 'MarkerEdgeColor', 'none', 'MarkerSize', 2);

% Configure camera
view(60,15) ;
grid on ;
axis equal ;

% Labels
title ( 'Path of Finger Towards Target Located at (0,0,0)' ) ; 
ylabel('Horizontal X [mm]') ;
zlabel('Vertical Y [mm]') ;
xlabel('Depth Z [mm]') ;
set(gcf, 'Color', 'w');  % 'w' is shorthand for white

% Extract limits from data
minX = ( ceil ( min(x) / 100 ) -1 ) * 100 ;
maxX = ( ceil ( max(x) / 100 ) +1 ) * 100 ;
limY = ( ceil ( max(abs(y)) / 100 )  ) * 100 ;
limZ = ( ceil ( max(abs(z)) / 100 )  ) * 100 ;

% Extract color information
zNorm = ( x - min(x)) / (max(x)- min(x)) ; 
zWeight = 1 - zNorm ;
colors = ( 1 - zWeight ) .* colStart + zWeight .* colEnd ;

% Set axis limits
xlim([minX maxX])            
xticks(minX:100:maxX);        
ylim([-limY limY])            
yticks(-limY:100:limY);        
zlim([-limZ limZ]) ;
zticks([-limY:100:limY]) ; 

% Reference axis
plot3([minX, minX], [-limY, limY] , [0, 0] , '-' , 'Color', colGre, 'LineWidth', 2) ; % X axis main
plot3([minX, minX], [0, 0] , [-limZ, limZ] , '-' , 'Color', colRed, 'LineWidth', 2) ; % Y axis
plot3([minX, maxX], [limY, limY] , [0, 0] , '-' , 'Color', colGre, 'LineWidth', .5) ; % X axis side
plot3([minX, maxX], [0, 0] , [-limZ, -limZ] , '-' , 'Color', colRed , 'LineWidth', .5) ; % Y axis size

hold off ;



% Top tile
xTile = nexttile ( t , [1 1] ) ; 
axes( xTile ) ; 
hold on ;

% Initialize plot
plotX = plot(xTile , NaN, NaN , '-', 'Color', colRed, 'LineWidth', 2);

% Set axis limits
xlim([0 time(end)])            
xticks(0:1:time(end));        
ylim([-limY limY])            
yticks(-limY:100:limY);        
grid on;

% Reference axis
plot([0, time(end)], [0, 0], '-' , 'Color', colRedLt , 'LineWidth', 2) ; % Y axis size

% Labels
title ( 'X Error' ) ; 
ylabel('X Error [mm]') ;
xlabel('Time [s]') ;
set(gcf, 'Color', 'w');  % 'w' is shorthand for white

hold off;



% Middle tile (Y)
yTile = nexttile ( t , [1 1] ) ; 
axes( yTile ) ; 
hold on ;

% Initialize plot
plotY = plot(yTile , NaN, NaN , '-', 'Color', colGre, 'LineWidth', 2);

% Set axis limits
xlim([0 time(end)])            
xticks(0:1:time(end));        
ylim([-limZ limZ])            
yticks(-limZ:100:limZ);        
grid on;

% Reference axis
plot([0, time(end)], [0, 0], '-' , 'Color', colGreLt , 'LineWidth', 2) ; % Y axis size

% Labels
title ( 'Y Error' ) ; 
ylabel('Y Error [mm]') ;
xlabel('Time [s]') ;
set(gcf, 'Color', 'w');  % 'w' is shorthand for white

hold off;



% Bottom tile (Z)
zTile = nexttile ( t , [1 1] ) ; 
axes( zTile ) ; 
hold on ;

% Initialize plot
plotZ = plot(zTile , NaN, NaN , '-', 'Color', colBlu, 'LineWidth', 2);

% Set axis limits
xlim([0 time(end)])            
xticks(0:1:time(end));        
ylim([0 maxX])            
yticks(0:100:maxX);        
grid on;

% Reference axis
plot([0, time(end)], [0, 0], '-' , 'Color', colBluLt , 'LineWidth', 2) ; % Y axis size

% Labels
title ( 'Z Error' ) ; 
ylabel('Z Error [mm]') ;
xlabel('Time [s]') ;
set(gcf, 'Color', 'w');  % 'w' is shorthand for white

hold off;

% After all plots are created and tiledlayout is complete:
drawnow; % Ensure layout positions are finalized


% Create videowriter
if ANIMATE 
    video = VideoWriter ( [filename '.mp4'] , 'MPEG-4' ) ; 
    fps = 1 / ( time(end) / length ( time ) ) 
    video.FrameRate = fps ; 
    open(video) ;
end

% Labels
timeLabelL = annotation('textbox', [0.005, 0.95, 0.2, 0.05], 'String', '', 'EdgeColor', 'none', 'FontSize', 12, 'Color', 'k', 'HorizontalAlignment', 'left', 'VerticalAlignment', 'top');
timeLabelR= annotation('textbox', [0.01, 0.95, 0.075, 0.05], 'String', '', 'EdgeColor', 'none', 'FontSize', 12, 'Color', 'k', 'HorizontalAlignment', 'right', 'VerticalAlignment', 'top');
errorLabelXL = annotation('textbox', [0.005, 0.92, 0.2, 0.05], 'String', '', 'EdgeColor', 'none', 'FontSize', 12, 'Color', colRedDk, 'HorizontalAlignment', 'left', 'VerticalAlignment', 'top');
errorLabelXR = annotation('textbox', [0.01, 0.92, 0.075, 0.05], 'String', '', 'EdgeColor', 'none', 'FontSize', 12, 'Color', colRedDk, 'HorizontalAlignment', 'right', 'VerticalAlignment', 'top');
errorLabelYL = annotation('textbox', [0.005, 0.89, 0.2, 0.05], 'String', '', 'EdgeColor', 'none', 'FontSize', 12, 'Color', colGreDk, 'HorizontalAlignment', 'left', 'VerticalAlignment', 'top');
errorLabelYR = annotation('textbox', [0.01, 0.89, 0.075, 0.05], 'String', '', 'EdgeColor', 'none', 'FontSize', 12, 'Color', colGreDk, 'HorizontalAlignment', 'right', 'VerticalAlignment', 'top');
errorLabelZL = annotation('textbox', [0.005, 0.86, 0.2, 0.05], 'String', '', 'EdgeColor', 'none', 'FontSize', 12, 'Color', colBlu, 'HorizontalAlignment', 'left', 'VerticalAlignment', 'top');
errorLabelZR = annotation('textbox', [0.01, 0.86, 0.075, 0.05], 'String', '', 'EdgeColor', 'none', 'FontSize', 12, 'Color', colBlu, 'HorizontalAlignment', 'right', 'VerticalAlignment', 'top');

% Labels for subplots
smallLabelX = annotation('textbox', [0.95, 0.81, 0.05, 0.05], 'String', '', 'EdgeColor', 'none', 'FontSize', 12, 'Color', colRedDk, 'HorizontalAlignment', 'right', 'VerticalAlignment', 'top');
smallLabelY = annotation('textbox', [0.95, 0.48, 0.05, 0.05], 'String', '', 'EdgeColor', 'none', 'FontSize', 12, 'Color', colGreDk, 'HorizontalAlignment', 'right', 'VerticalAlignment', 'top');
smallLabelZ = annotation('textbox', [0.95, 0.025, 0.05, 0.05], 'String', '', 'EdgeColor', 'none', 'FontSize', 12, 'Color', colBluDk, 'HorizontalAlignment', 'right', 'VerticalAlignment', 'top');

% Borders
annotation ( 'rectangle', [0, 0, 1.0, 1.0] , 'Color', colGraLt) ;
annotation ( 'line', [0.5, 0.5], [0, 1.0] , 'Color', colGraLt ) ;
annotation ( 'line', [0.5, 1], [0.66, 0.66] , 'Color', colGraLt ) ;
annotation ( 'line', [0.5, 1], [0.33, 0.33] , 'Color', colGraLt) ;

    % plot3(x(i), y(i), -limZ, 'o', 'MarkerFaceColor', colRedLt, 'MarkerEdgeColor', 'none', 'MarkerSize', 2);
    % plot3(minX, y(i), z(i), 'o', 'MarkerFaceColor', colBluLt, 'MarkerEdgeColor', 'none', 'MarkerSize', 2);



% Animate
for i = 1:length(time)
  
    % Update telemetry plot and shadows
    set ( telemetryPlot , 'XData' , x(1:i) , 'YData' , y(1:i) , 'ZData' , z(1:i) , 'MarkerFaceColor', colPin, 'MarkerEdgeColor', 'none', 'MarkerSize', 3) ;
    set ( telemetryShadowPlotY , 'XData' , x(1:i) , 'YData' , repmat(limY, 1, i), 'ZData' , z(1:i) , 'MarkerFaceColor', colGreLt, 'MarkerEdgeColor', 'none', 'MarkerSize', 2);
    set ( telemetryShadowPlotZ , 'XData' , x(1:i) , 'YData' , y(1:i), 'ZData' , repmat(-limZ, 1, i) , 'MarkerFaceColor', colRedLt, 'MarkerEdgeColor', 'none', 'MarkerSize', 2);
    set ( telemetryShadowPlotX , 'XData' , repmat(minX, 1, i) , 'YData' , y(1:i), 'ZData' , z(1:i) , 'MarkerFaceColor', colBluLt, 'MarkerEdgeColor', 'none', 'MarkerSize', 2);
    
    % Update XYZ plots
    set ( plotX, 'XData' , time(1:i) , 'YData' , y(1:i) );
    set ( plotY, 'XData' , time(1:i) , 'YData' , z(1:i) );
    set ( plotZ, 'XData' , time(1:i) , 'YData' , x(1:i) );
    
        
        % plot3(x(i), limY, z(i), 'o', 'MarkerFaceColor', colGreLt, 'MarkerEdgeColor', 'none', 'MarkerSize', 2);
    % plot3(x(i), y(i), -limZ, 'o', 'MarkerFaceColor', colRedLt, 'MarkerEdgeColor', 'none', 'MarkerSize', 2);
    % plot3(minX, y(i), z(i), 'o', 'MarkerFaceColor', colBluLt, 'MarkerEdgeColor', 'none', 'MarkerSize', 2);

    % Update frame
    drawnow ;

    % Inside your animation loop
    
    timeLabelL.String = sprintf('Time:');
    timeLabelR.String = sprintf('%.2f s', time(i));
    errorLabelXL.String = sprintf('Error X:');
    errorLabelXR.String = sprintf('%.0f mm', y(i));
    errorLabelYL.String = sprintf('Error Y:');
    errorLabelYR.String = sprintf('%.0f mm', z(i));
    errorLabelZL.String = sprintf('Error Z:');
    errorLabelZR.String = sprintf('%.0f mm', x(i));
    
    smallLabelX.String = sprintf('%.0f mm', y(i));
    smallLabelY.String = sprintf('%.0f mm', z(i));
    smallLabelZ.String = sprintf('%.0f mm', x(i));

    % Save frame
    if ANIMATE
        frame = getframe(gcf) ; 
        writeVideo ( video , frame ) ; 
       
        % Pause
        if i > 1
            pause ( time(i) - time(i-1) ) ;
        end
    end

    
end

if ANIMATE
    close ( video ) ;    
end

disp( "Done!" ) ;
