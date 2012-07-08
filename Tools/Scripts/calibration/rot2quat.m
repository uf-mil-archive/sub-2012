function q = rot2quat(R)
%% Source of Algorithm : New Method for Extracting the Quaternion from a
%% Rotation Matrix by Itzhack Y. Bar-Itzhack 
%% (Journal of GUIDANCE, VOL. 23, NO. 6: ENGINEERING NOTES)
% Most forumale for Extracting the Quaternion from a Rotation Matrix
% expect the rotation angle to be very small. Shepperd’s algorithm is 
% the simplest and most popular algorithm that holds even for large rotation angles, 
% but it requires a square root computation and a certain voting in the way the
% quaternion elements are computed. 

% Bar-Itzhack suggests this new algorithm, for extracting the
% quaternion from the corresponding Rotation Matrix, which is valid for 
% all attitudes and does not require any voting.Moreover, if the given 
% Rotation Matrix is not precise and, thereby, is not orthogonal, it 
% yields the optimal quaternion in the sense that it is the quaternion 
% that corresponds to the orthogonal matrix closest to the given imprecise
% Rotation Matrix.

%% Bar-Itzhack's algo follows

K =  [R(1,1)-R(2,2)-R(3,3),  R(2,1)+R(1,2), R(3,1)+R(1,3),  R(2,3)-R(3,2);...
      R(2,1)+ R(1,2),  R(2,2)-R(1,1)-R(3,3),  R(3,2)+R(2,3), R(3,1)-R(1,3);...
      R(3,1)+R(1,3),   R(3,2)+R(2,3),  R(3,3)-R(1,1)-R(2,2),  R(1,2)-R(2,1);...
      R(2,3)-R(3,2),   R(3,1)-R(1,3),  R(1,2)-R(2,1),   R(1,1)+R(2,2)+R(3,3)];
K3 = (1/3) * K;  
[V D] = eig(K3);
eigen_K3 = [0 0 0 0];
for i = 1:4
    for j = 1:4
        eigen_K3(i) = eigen_K3(i) + D(j,i);
    end
end
max_index = find(eigen_K3 == max(eigen_K3));
q_bar = V(1:4,max_index);
q = [q_bar(4);q_bar(1);q_bar(2);q_bar(3)]; % the algo uses q = [qv;qs] notation
% but we use q = [qs;qv] notation to represent quaternions
q = quatconj(q); % the algorithm gives quaternion correspoding to inv(R)
q = quatnorm(q); % in case the result is not normalized, normalize it here
%q = -q; % we know that -q is same as q but we always try to have +ve elements
%%
end
