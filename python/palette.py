# Source: https://stackoverflow.com/a/33597599/5391200

from pylab import *
import matplotlib

# cmap = cm.get_cmap('seismic', 5)  # matplotlib color palette name, n colors
# for i in range(cmap.N):
#     rgb = cmap(i)[:3]  # will return rgba, we take only first 3 so we get rgb
#     print(matplotlib.colors.rgb2hex(rgb))
#
# # cmap = cm.get_cmap('plasma', 101)

cmap = matplotlib.colormaps['plasma']
print("auto const colormap = {")
for i in range(30):
    c = matplotlib.colors.to_rgba_array(cmap(i / 30))[0]
    print("G4Colour(", c[0], ",", c[1], ",", c[2], "),")

print("};")
