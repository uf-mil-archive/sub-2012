% Generates a valid hydrophone.config from the following settings

scalefact = 30;
samplingrate = 240382;
soundvelocity = 1500;
datasize = 256;
disth = (0.9*2.54)/100;
disth4 = disth;

bandpass_coefs = fir1(31,[((20e3)/(samplingrate/2)) ((30e3)/(samplingrate/2))], 'bandpass');

data = zeros(datasize);
[data_upsamp, upsample_coefs] = resample(data, scalefact, 1); % save the lowpass filter matlab internally generates to perform resampling

hamming_coefs = hamming(datasize);

fd = fopen('hydrophone.config', 'w');
fprintf(fd, '<scalefact>%d</scalefact>\n', scalefact);
fprintf(fd, '<samplingrate>%d</samplingrate>\n', samplingrate);
fprintf(fd, '<soundvelocity>%d</soundvelocity>\n', soundvelocity);
fprintf(fd, '<disth>%e</disth>\n', disth);
fprintf(fd, '<disth4>%e</disth>\n', disth4);

fprintf(fd, '<bandpass>\n');
for i=1:length(bandpass_coefs)
    fprintf(fd, '%.16e ', bandpass_coefs(i));
end
fprintf(fd, '\n</bandpass>\n');

fprintf(fd, '<upsample>\n');
for i=1:length(upsample_coefs)
    fprintf(fd, '%.16e ', upsample_coefs(i));
end
fprintf(fd, '\n</upsample>\n');

fprintf(fd, '<hamming>\n');
for i=1:length(hamming_coefs)
    fprintf(fd, '%.16e ', hamming_coefs(i));
end
fprintf(fd, '\n</hamming>\n');
fclose(fd);

