function J = get_J(q, lambda, src_image_name, dst_image_name, crs_image_name, src_image)

cmd = sprintf('.\\cwrap.exe %s %s %.3f %.3f', src_image_name, dst_image_name, q, lambda);
disp(['cmd:', cmd]);
system(cmd);
dst_image = imread(dst_image_name);
dst_image = double(dst_image(:, :, 1));

D = sum(sum((dst_image - src_image).^2));
psnr = 10 * log10(255^2 * numel(src_image) / D);
s = dir(crs_image_name);
R = s.bytes * 8;
bpp = R / numel(src_image);

J = D + lambda * R;

delete(dst_image_name);
delete(crs_image_name);

disp(sprintf('PSNR: %.2f bpp: %.2f J: %.2f', [psnr, bpp, J]));