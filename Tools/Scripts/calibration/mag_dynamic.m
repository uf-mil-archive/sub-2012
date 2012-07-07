close all;
clear all;
clc;

format long;

input_file_name = 'dynamic_cal_thruster_';
output_file_name = 'dynamic_cal_coeff.txt';

file_out = fopen(output_file_name, 'w');
fprintf(file_out, "{\n");
fprintf(file_out, "\t\"currentconfigs\": {\n");

magX_pos = 1;
magY_pos = 2;
magZ_pos = 3;
effort_pos = 4;
current_pos = 5;

fit_order = 3;

for thruster_num = 0:7

    data = csvread([input_file_name sprintf('%d',thruster_num) '.csv']);

    [rowD colD] = size(data);

    i = 2;

    while data(i-1,effort_pos) <= data(i,effort_pos)
        i = i + 1;
    end

    data_start = 1;
    data_end = i;

    forward_data = data(data_start:data_end,:);

    while data(i,effort_pos) ~= 0
        i = i + 1;
    end


    data_start = i;

    while data(i-1,effort_pos)>=data(i,effort_pos)
        i = i + 1;
    end

    data_end = i;

    reverse_data = data(data_start:data_end,:);


    f_current = forward_data(:,current_pos);
    f_magX = forward_data(:,magX_pos);
    f_magY = forward_data(:,magY_pos);
    f_magZ = forward_data(:,magZ_pos);

    r_current = reverse_data(:,current_pos);
    r_magX = reverse_data(:,magX_pos);
    r_magY = reverse_data(:,magY_pos);
    r_magZ = reverse_data(:,magZ_pos);

    f_magX_coeff = polyfit(f_current,f_magX,fit_order)
    f_magX_fit = polyval(f_magX_coeff,f_current);
    figure;
    hold on;
    plot(f_current, f_magX, 'b.');
    plot(f_current, f_magX_fit, 'r.');
    hold off;
    legend('magXcompass', 'magXcompassFit');
    title('magXcompass as a function of Current');
    xlabel('Current(Amps)');

    f_magY_coeff = polyfit(f_current,f_magY,fit_order)
    f_magY_fit = polyval(f_magY_coeff,f_current);
    figure;
    hold on;
    plot(f_current, f_magY, 'b.');
    plot(f_current, f_magY_fit, 'r.');
    hold off;
    legend('magYcompass', 'magYcompassFit');
    title('magYcompass as a function of Current');
    xlabel('Current(Amps)');

    f_magZ_coeff = polyfit(f_current,f_magZ,fit_order)
    f_magZ_fit = polyval(f_magZ_coeff,f_current);
    figure;
    hold on;
    plot(f_current, f_magZ, 'b.');
    plot(f_current, f_magZ_fit, 'r.');
    hold off;
    legend('magZcompass', 'magZcompassFit');
    title('magZcompass as a function of Current');
    xlabel('Current(Amps)');

    r_magX_coeff = polyfit(r_current,r_magX,fit_order)
    r_magX_fit = polyval(r_magX_coeff,r_current);
    figure;
    hold on;
    plot(r_current, r_magX, 'b.');
    plot(r_current, r_magX_fit, 'r.');
    hold off;
    legend('magXcompass', 'magXcompassFit');
    title('magXcompass as a function of Current');
    xlabel('Current(Amps)');

    r_magY_coeff = polyfit(r_current,r_magY,fit_order)
    r_magY_fit = polyval(r_magY_coeff,r_current);
    figure;
    hold on;
    plot(r_current, r_magY, 'b.');
    plot(r_current, r_magY_fit, 'r.');
    hold off;
    legend('magYcompass', 'magYcompassFit');
    title('magYcompass as a function of Current');
    xlabel('Current(Amps)');

    r_magZ_coeff = polyfit(r_current,r_magZ,fit_order)
    r_magZ_fit = polyval(r_magZ_coeff,r_current);
    figure;
    hold on;
    plot(r_current, r_magZ, 'b.');
    plot(r_current, r_magZ_fit, 'r.');
    hold off;
    legend('magZcompass', 'magZcompassFit');
    title('magZcompass as a function of Current');
    xlabel('Current(Amps)');

    fprintf(file_out, "\t\t\"%d\": {\n", thruster_num);
	fprintf(file_out, "\t\t\t\"forward\": [");
	fprintf(file_out, "\"0.0 0.0 0.0\"");
	for i=fliplr(1:fit_order)
		fprintf(file_out, ", ");
		fprintf(file_out, "\"%.12e %.12e %.12e\"", f_magX_coeff(i), f_magY_coeff(i), f_magZ_coeff(i));
	endfor
	fprintf(file_out, "],\n");
	fprintf(file_out, "\t\t\t\"reverse\": [");
	fprintf(file_out, "\"0.0 0.0 0.0\"");
	for i=fliplr(1:fit_order)
		fprintf(file_out, ", ");
		fprintf(file_out, "\"%.12e %.12e %.12e\"", r_magX_coeff(i), r_magY_coeff(i), r_magZ_coeff(i));
	endfor
	fprintf(file_out, "]\n");
	if thruster_num != 8
		fprintf(file_out, "\t\t},\n");
	else
		fprintf(file_out, "\t\t}\n");
	endif
end

fprintf(file_out, "\t}\n");
fclose(file_out);
