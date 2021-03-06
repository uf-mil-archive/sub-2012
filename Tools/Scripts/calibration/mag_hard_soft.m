clear all;
close all;
clc;

format long;

data = csvread('hard_soft.csv');
down_sample = data(1:10:length(data),:);

figure;
scatter3(down_sample(:,1),down_sample(:,2),down_sample(:,3),'r');

A = zeros(size(data,1), 9);
A(:,1) = data(:,1).^2;
A(:,2) = data(:,2).^2;
A(:,3) = data(:,3).^2;
A(:,4) = 2*data(:,1).*data(:,2);
A(:,5) = 2*data(:,1).*data(:,3);
A(:,6) = 2*data(:,2).*data(:,3);
A(:,7) = data(:,1);
A(:,8) = data(:,2);
A(:,9) = data(:,3);

B = ones(size(data,1), 1);

X = inv(A'*A)*A'*B;
Xorig = X;


error = A(:,1).*X(1) + A(:,2).*X(2) + A(:,3).*X(3) + ...
    A(:,4).*X(4) + A(:,5).*X(5) + A(:,6).*X(6) + ...
    A(:,7).*X(7) +A(:,8).*X(8) + A(:,9).*X(9);

error = error - 1;
mean(abs(error));
max(abs(error));
mse = mean(error.^2)


%Find the center of ellipsoid
a = [X(1) X(4) X(5); X(4) X(2) X(6); X(5) X(6) X(3)];
b = [X(7); X(8); X(9)];

[P1,a2,P2] = svd(a);

b2 = (b'*P1)';

x0 = (b2 ./ diag(a2))*.5;

shift = P1*x0
rot = P1

q = rot2quat(rot)

c = 1 + (x0(1))^2*a2(1,1) + (x0(2))^2*a2(2,2) + (x0(3))^2*a2(3,3);

%Find the 1/2 lenght of all "major" axis of the ellipsoid
Major1 = sqrt(c/a2(1,1))
Major2 = sqrt(c/a2(2,2))
Major3 = sqrt(c/a2(3,3))

file = fopen("mag_hard_soft.txt", "w");
fprintf(file, "\t\"q_MagCorrection\": \"%.12e %.12e %.12e %.12e\",\n", q(1), q(2), q(3), q(4));
fprintf(file, "\t\"magShift\": \"%.12e %.12e %.12e\",\n", shift(1), shift(2), shift(3));
fprintf(file, "\t\"magScale\": \"%.12e %.12e %.12e\",\n", Major1, Major2, Major2);
fclose(file);
