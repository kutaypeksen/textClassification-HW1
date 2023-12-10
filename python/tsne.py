import matplotlib.pyplot as plt

def dosyadan_y_oku(dosya_adı):
    y_degerleri = []

    with open(dosya_adı, 'r') as dosya:
        for satir in dosya:
            y = float(satir.strip())
            y_degerleri.append(y)

    return y_degerleri

def grafik_ciz(dosya_listesi):
    plt.xlabel('Epoch')
    plt.ylabel('Error')

    for dosya_adı, label in dosya_listesi:
        y_degerleri = dosyadan_y_oku(dosya_adı)
        x_degerleri = list(range(1, len(y_degerleri) + 1))
        plt.plot(x_degerleri, y_degerleri, label=label)

    plt.legend()
    plt.title('LOSS-EPOCH GRAPHIC')
    plt.show()

# Dosya adları ve etiketleri
dosya_listesi = [
    (r'C:\Users\isims\OneDrive\Masaüstü\diff\GDerror.txt', 'GD'),
    (r'C:\Users\isims\OneDrive\Masaüstü\diff\SGDerror.txt', 'SGD'),
    (r'C:\Users\isims\OneDrive\Masaüstü\diff\ADAMerror.txt', 'ADAM'),
]

# Grafik çizimi
grafik_ciz(dosya_listesi)
