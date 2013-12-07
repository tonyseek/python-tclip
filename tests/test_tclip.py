from wand.image import Image
from py.path import local
from pytest import fixture, raises
from tclip import TClip


@fixture
def tclip_sample(tmpdir):
    src_path = local(__file__).dirpath("data/alan-turing.jpg")
    dst_path = tmpdir.mkdir("tclip-test").join("alan-turing-200x200.jpg")
    return src_path, dst_path


def test_can_work(tclip_sample):
    src_path, dst_path = tclip_sample
    clip = TClip(200, 200)

    with Image(filename=str(src_path)) as image:
        assert abs(image.width - 200) > 5
        assert abs(image.height - 200) > 5

    assert clip.process_file(str(src_path), str(dst_path)) == 0

    with Image(filename=str(dst_path)) as image:
        assert abs(image.width - 200) < 5
        assert abs(image.height - 200) < 5


def test_missing_config(tclip_sample):
    src_path, dst_path = tclip_sample
    clip = TClip(200, 200)
    clip.config_path = None

    with raises(RuntimeError):
        clip.process_file(str(src_path), str(dst_path))
