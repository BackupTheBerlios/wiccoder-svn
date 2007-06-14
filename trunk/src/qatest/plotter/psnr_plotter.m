function psnr_plotter(src_image_name)

clc

crs_image_name = 'temp.out';
dst_image_name = sprintf('%s_restored%s', src_image_name(1 : end - 4), src_image_name(end - 3 : end));
src_image_name = sprintf('..\\..\\..\\res\\images\\%s', src_image_name);
src_image = double(imread(src_image_name));
src_image = src_image(:, :, 1);

result = [];
for q = 12 : 1 : 15
    psnr = [];
    bpp = [];
    for lambda = 5 : 5 : 30
        system(sprintf('..\\..\\..\\bin\\cwrap.exe %s %s %f %f', src_image_name, dst_image_name, q, lambda));
        dst_image = double(imread(dst_image_name));
        dst_image = dst_image(:, :, 1);

        psnr = [psnr 10 * log10(255^2 * numel(src_image) / sum(sum((dst_image - src_image).^2)))];
        s =  dir(crs_image_name);
        bpp = [bpp s.bytes * 8 / numel(src_image)];

        result = [result struct('q', q, 'lambda', lambda, 'psnr', psnr(end), 'bpp', bpp(end))];
        delete(dst_image_name);
        delete(crs_image_name);
    end
    plot(bpp, psnr, '-ks', 'MarkerFaceColor', 'k', 'MarkerSize', 4), hold on, grid on
    text(bpp(1) + .01, psnr(1), sprintf('q = %d', q), 'HorizontalAlignment','left',  'BackgroundColor', 'white')
end

title(sprintf('%s; lambda = [ %s ]', src_image_name(21:end), num2str(unique([result.lambda]))));
xlabel('bpp');
ylabel('psnr');

[[result.q]' [result.lambda]' [result.psnr]' [result.bpp]']