import matplotlib.pyplot as plt

def dosyadan_x_y_oku(x_dosya, y_dosya, label):
    x_degerleri = []
    y_degerleri = []

    with open(x_dosya, 'r') as xdosya, open(y_dosya, 'r') as ydosya:
        for x, y in zip(xdosya, ydosya):
            x_degerleri.append(float(x.strip()))
            y_degerleri.append(float(y.strip()))

    return x_degerleri, y_degerleri, label

def grafik_ciz(data_list):
    plt.xlabel('Time')
    plt.ylabel('Loss')
    
    for x_degerleri, y_degerleri, label in data_list:
        plt.plot(x_degerleri, y_degerleri, label=label)

    plt.legend()
    plt.title('LOSS-TIME GRAPHIC')
    plt.show()

# Dosya adlarını belirtin
dosya_listesi = [
    (r'C:\Users\isims\OneDrive\Masaüstü\diff\GDtime.txt', r'C:\Users\isims\OneDrive\Masaüstü\diff\GDerror.txt', 'GD'),
    (r'C:\Users\isims\OneDrive\Masaüstü\diff\SGDtime.txt', r'C:\Users\isims\OneDrive\Masaüstü\diff\SGDerror.txt', 'SGD'),
    (r'C:\Users\isims\OneDrive\Masaüstü\diff\ADAMtime.txt', r'C:\Users\isims\OneDrive\Masaüstü\diff\ADAMerror.txt', 'ADAM'),
]

# Dosyadan verileri oku
data_list = [dosyadan_x_y_oku(x_dosya, y_dosya, label) for x_dosya, y_dosya, label in dosya_listesi]

# Grafiği çiz
grafik_ciz(data_list)
