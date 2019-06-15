import linedraw.linedraw as linedraw


def developement_main():
    lines = linedraw.sketch("images_to_draw/alphabet.png")  # return list of polylines, eg.
    print(lines)


if __name__ == '__main__':
    developement_main()
