package com.dreizak.miniball.highdim;

import static junit.framework.Assert.assertEquals;
import static junit.framework.Assert.assertTrue;

import java.io.IOException;
import java.io.InputStream;

import org.junit.Ignore;
import org.junit.Test;

import com.dreizak.miniball.model.PointSet;
import com.dreizak.miniball.model.PointSetUtils;

public class MiniballTest
{
  @Test
  public void test_almost_cospherical_points_3() throws IOException
  {
    Miniball mb = computeFromFile(getClass().getResourceAsStream("data/almost_cospherical_points_3.data"));
    double[] expectedCenter = {
        -1.56087201342490318e-12, -4.71446026502953592e-12, 3.67580472387244665e-12
    };
    assertEquals(10000, mb.size());
    assertAlmostEquals(expectedCenter, mb.center());
    assertAlmostEquals(1.00000000015068569e+00, mb.squaredRadius());
  }

  @Test
  public void test_almost_cospherical_points_10() throws IOException
  {
    Miniball mb = computeFromFile(getClass().getResourceAsStream("data/almost_cospherical_points_10.data"));
    double[] expectedCenter = {
        5.87465071980114019e-12,
        -8.50168966753148976e-12,
        5.16296955479470709e-12,
        5.76827248197897234e-12,
        4.79834156807867167e-12,
        3.60221381957526638e-13,
        -5.39518968945866875e-12,
        1.10109862800149464e-11,
        5.92724627952881232e-12,
        3.08121800360276428e-12
    };
    assertEquals(10000, mb.size());
    assertAlmostEquals(expectedCenter, mb.center());
    assertAlmostEquals(1.00000000016537482e+00, mb.squaredRadius());
  }

  @Test
  public void test_cocircular_points_large_radius_2() throws IOException
  {
    Miniball mb = computeFromFile(getClass().getResourceAsStream("data/cocircular_points_large_radius_2.data"));
    double[] expectedCenter = {
        2.25917732813639420e-08, 2.66532983586183870e-08
    };
    assertEquals(13824, mb.size());
    assertAlmostEquals(expectedCenter, mb.center());
    assertAlmostEquals(1.12830550249511283e+18, mb.squaredRadius());
  }

  @Test
  public void test_cocircular_points_small_radius_2() throws IOException
  {
    Miniball mb = computeFromFile(getClass().getResourceAsStream("data/cocircular_points_small_radius_2.data"));
    double[] expectedCenter = {
        0, 0
    };
    assertEquals(6144, mb.size());
    assertAlmostEquals(expectedCenter, mb.center());
    assertAlmostEquals(3.72870291637512500e+15, mb.squaredRadius());
  }

  @Test
  public void test_cube_10() throws IOException
  {
    Miniball mb = computeFromFile(getClass().getResourceAsStream("data/cube_10.data"));
    double[] expectedCenter = {
        0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
    };
    assertEquals(1024, mb.size());
    assertAlmostEquals(expectedCenter, mb.center());
    assertAlmostEquals(2.5, mb.squaredRadius());
  }

  @Test
  public void test_cube_12() throws IOException
  {
    Miniball mb = computeFromFile(getClass().getResourceAsStream("data/cube_12.data"));
    double[] expectedCenter = {
        0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5
    };
    assertEquals(4096, mb.size());
    assertAlmostEquals(expectedCenter, mb.center());
    assertAlmostEquals(3, mb.squaredRadius());
  }

  @Test
  public void test_hurz() throws IOException
  {
    Miniball mb = computeFromFile(getClass().getResourceAsStream("data/hurz.data"));
    double[] expectedCenter = {
        -9.32385490743424666e-02,
        -1.50055661837044246e-01,
        2.69364248349470529e-02,
        -2.40723461491440050e-03,
        -6.73931277729599293e-02,
        5.48757211321871177e-02,
        -3.77507874171383526e-02,
        1.52069101200107918e-02,
        3.17372740475474271e-02,
        1.19705682509647776e-01,
        2.30776967046545170e-02,
        1.34715212599093803e-01,
        -6.77742873939496671e-02,
        8.15765590285277495e-03,
        9.10084642845849501e-02,
        7.59318418646900695e-02,
        6.67014412781230803e-02,
        2.43008063975082983e-02,
        5.75994499483733213e-02,
        2.71948026809261494e-02,
        8.93012380869349226e-03,
        6.78549081409481059e-03,
        -3.91898096095426723e-02,
        2.61203895241791072e-02,
        1.30183917339636385e-01,
        -9.05159956449119174e-02,
        8.29177344908292452e-02,
        -1.01263907022741025e-02,
        7.26859883818094521e-02,
        1.50591656331595884e-02,
        -6.68358467554956426e-02,
        1.32664103589673517e-02,
        5.26161351518988074e-02,
        -5.95612639904603055e-03,
        1.83611423209012921e-02,
        5.87399305108671430e-02,
        2.88703317032385394e-02,
        -7.63799904284965109e-02,
        -3.91152746123283185e-02,
        -3.89767810320450492e-02,
        2.35807392210171836e-02,
        -2.03958450688055126e-02,
        -5.39645593710812513e-02,
        -1.59803577488685754e-02,
        6.80661505439016240e-02,
        -2.98838443491668999e-02,
        6.29793667302477600e-02,
        -5.06384997867233358e-02,
        -1.12101972723559248e-01,
        2.63368521873624216e-02,
        -3.07658266670295631e-02,
        -2.64456299786865051e-02,
        1.48886115569620697e-01,
        6.39413771405114711e-02,
        3.46874273101637105e-03,
        5.04544870734000214e-02,
        -7.23303966655623881e-02,
        6.69173418830567995e-02,
        -2.05141517773424237e-01,
        1.53060995099086744e-01,
        1.09764268432820569e-01,
        -4.15657204407630654e-02,
        5.59111099323817001e-02,
        -1.08878576328361426e-01,
        6.72514700094211659e-02,
        5.67031662004510370e-02,
        2.98503270757259527e-02,
        -2.22211414794732769e-02,
        1.17699103704017344e-01,
        -7.74243833358304601e-02,
        1.14144909884642876e-01,
        1.18244885236012495e-01,
        -1.16498953956752041e-01,
        -1.54335227921318263e-01,
        -1.27061966479000893e-01,
        4.93839729837722702e-02,
        9.65276608923077567e-02,
        6.87789555146762766e-02,
        8.02775373675647941e-02,
        -2.95398290193190638e-02,
        1.00148883621633628e-01,
        1.41660341680982599e-01,
        -1.13077032939844724e-01,
        -9.92412686510760061e-02,
        -7.34660260569389156e-02,
        2.44119317172067987e-02,
        -8.60823590551833689e-02,
        4.81621197168055418e-02,
        2.13685862730959644e-02,
        -2.10485109618934463e-02,
        1.25159956599585509e-01,
        -9.81387077643431638e-02,
        7.39057728784511953e-02,
        -5.74760989576298745e-02,
        9.02697615359867450e-02,
        -7.13332356872704767e-03,
        -4.58852458824499237e-02,
        -4.63562081793597661e-02,
        5.72641924237869832e-02,
        5.81975472026072596e-02,
        9.24491451791044916e-02,
        -9.11941779278649667e-02,
        1.30119072412531483e-01,
        1.36620780424713567e-01,
        5.49437946994311352e-02,
        -8.13593869366321526e-02,
        8.82168141230831315e-02,
        1.63874559179065585e-02,
        6.33525944638172467e-02,
        -7.78800446589289624e-02,
        1.05095502046718692e-01,
        6.49385016232090495e-02,
        -8.05548020284199695e-02,
        -7.51567403689807900e-02,
        5.64638210880798116e-02,
        3.20522130857825993e-02,
        -9.09777076137542745e-02,
        -9.76163942088352082e-02,
        -6.18280846431228312e-02,
        -1.31709582117138319e-01,
        -1.97527865667240527e-03,
        7.21931497585995285e-03,
        -4.20086728084883079e-02,
        -1.97727359026356797e-01,
        9.60228739542908459e-03,
        1.16114754875734708e-01,
        1.34450809189754477e-02,
        -4.53777260307672362e-02,
        -3.16343163102534713e-02,
        1.28342389158762504e-01,
        3.22817704875962702e-02,
        1.54647738406440208e-02,
        1.47101218437264564e-01,
        2.15748587532079462e-02,
        1.14410676952691956e-01,
        -5.61923008472568625e-02,
        5.33700224338453030e-02,
        2.68966630713247251e-02,
        -3.85872036409353192e-02,
        3.38504979473726250e-02,
        -6.20998780759002694e-02,
        2.05930310272340843e-02,
        2.62773470028755222e-02,
        -1.54454477761890429e-02,
        7.57992611852749548e-02,
        2.73988468770113916e-02,
        8.65941702756104076e-02,
        5.07588096831466246e-02,
        -6.93803682141353462e-02,
        4.81234642042299016e-02,
        1.30443064335880442e-02,
        8.27268777941885247e-02,
        -9.16112442109258546e-02,
        -4.45938341297191718e-02,
        8.33022514572618299e-03,
        -5.50105572034664814e-03,
        3.42063053108011871e-03,
        2.62405615322419770e-03,
        6.01664324087401378e-02,
        -2.07679618603799840e-02,
        2.85058169191099720e-02,
        -1.09744548510173290e-01,
        -7.77693036824233380e-02,
        1.02404605592325412e-03,
        -1.42314434178243930e-02,
        -5.62917711091049031e-02,
        1.26842625168207629e-01,
        -3.14995069731299759e-02,
        -2.32973697443584604e-03,
        -1.04374526387181921e-01,
        -8.16693564187557230e-02,
        4.21643856974714057e-02,
        3.82603433374553228e-02,
        7.30422312859391326e-02,
        1.21749543666098747e-01,
        6.11472483977238065e-02,
        3.84288077038831277e-02,
        -7.17878918962588375e-02,
        -5.64552109618581682e-02,
        1.60817706579412106e-01,
        2.99709054001969129e-02,
        3.41409563710960914e-02,
        -1.75688728854233277e-02,
        -2.63404757837025147e-02,
        1.69281593990837642e-02,
        7.60971983283898479e-02,
        -1.81655270703341247e-02,
        3.49304205730180256e-02,
        1.92995515142081568e-03,
        2.16056157812245936e-02,
        -5.76236634233693035e-02,
        -7.61103361807527121e-03,
        -1.64553293154055154e-02,
        -4.08614772137683158e-02,
        2.29673439914067565e-02,
        -7.45104306720340281e-02,
        2.41470698178044355e-02,
        -8.83633789907483091e-02,
        -5.45070973771131506e-02,
        4.86914400373976639e-02
    };
    assertEquals(800, mb.size());
    assertAlmostEquals(expectedCenter, mb.center());
    assertAlmostEquals(7.38649825408094216e+01, mb.squaredRadius());
  }

  @Test
  public void test_longitude_latitude_model_3() throws IOException
  {
    Miniball mb = computeFromFile(getClass().getResourceAsStream("data/longitude_latitude_model_3.data"));
    double[] expectedCenter = {
        1.09529643705128193e-12, 7.76487134128295763e-12, 2.09782485152182732e-16
    };
    assertEquals(10000, mb.size());
    assertAlmostEquals(expectedCenter, mb.center());
    assertAlmostEquals(1.00000000015833201e+00, mb.squaredRadius());
  }

  @Test
  public void test_random_points_3() throws IOException
  {
    Miniball mb = computeFromFile(getClass().getResourceAsStream("data/random_points_3.data"));
    double[] expectedCenter = {
        1.61093103034421342e-02, -3.39403799912667802e-03, -3.31378412407934593e-03
    };
    assertEquals(10000, mb.size());
    assertAlmostEquals(expectedCenter, mb.center());
    assertAlmostEquals(6.86700124388323507e-01, mb.squaredRadius());
  }

  @Test
  public void test_random_points_5() throws IOException
  {
    Miniball mb = computeFromFile(getClass().getResourceAsStream("data/random_points_5.data"));
    double[] expectedCenter = {
        1.68879606901127695e-02,
        -5.59476980486128209e-03,
        -3.50733482301275185e-02,
        4.97024582833288132e-05,
        -1.64470438482790457e-02
    };
    assertEquals(10000, mb.size());
    assertAlmostEquals(expectedCenter, mb.center());
    assertAlmostEquals(9.86950425115668661e-01, mb.squaredRadius());
  }

  @Test
  public void test_random_points_10() throws IOException
  {
    Miniball mb = computeFromFile(getClass().getResourceAsStream("data/random_points_10.data"));
    double[] expectedCenter = {
        -3.68108604382322915e-02,
        -2.56379744386364197e-02,
        2.63525258190180009e-02,
        -4.39111259663507118e-03,
        5.07655020786604433e-02,
        2.46652284563754105e-02,
        6.32648262334569861e-02,
        1.72315689301736075e-02,
        -1.20835421442481560e-02,
        6.39141412880165488e-03
    };
    assertEquals(10000, mb.size());
    assertAlmostEquals(expectedCenter, mb.center());
    assertAlmostEquals(1.62004498782482909e+00, mb.squaredRadius());
  }

  @Ignore
  @Test
  public void test_schnarz() throws IOException // TODO: see #6
  {
    Miniball mb = computeFromFile(getClass().getResourceAsStream("data/schnarz.data"));
    double[] expectedCenter = {
        0, 0
    };
    System.err.println("schnarz " + mb);
    assertEquals(10000, mb.size());
    assertAlmostEquals(expectedCenter, mb.center());
    assertAlmostEquals(0, mb.squaredRadius());
  }

  @Test
  public void test_simplex_10() throws IOException
  {
    Miniball mb = computeFromFile(getClass().getResourceAsStream("data/simplex_10.data"));
    double[] expectedCenter = {
        0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1
    };
    assertEquals(10, mb.size());
    assertAlmostEquals(expectedCenter, mb.center());
    assertAlmostEquals(1 - 1 / 10.0, mb.squaredRadius());
  }

  @Test
  public void test_simplex_15() throws IOException
  {
    Miniball mb = computeFromFile(getClass().getResourceAsStream("data/simplex_15.data"));
    double[] expectedCenter = {
        1 / 15.0,
        1 / 15.0,
        1 / 15.0,
        1 / 15.0,
        1 / 15.0,
        1 / 15.0,
        1 / 15.0,
        1 / 15.0,
        1 / 15.0,
        1 / 15.0,
        1 / 15.0,
        1 / 15.0,
        1 / 15.0,
        1 / 15.0,
        1 / 15.0,
    };
    assertEquals(15, mb.size());
    assertAlmostEquals(expectedCenter, mb.center());
    assertAlmostEquals(1 - 1 / 15.0, mb.squaredRadius());
  }

  Miniball computeFromFile(InputStream s) throws IOException
  {
    PointSet pts = PointSetUtils.pointsFromStream(s);
    Miniball mb = new Miniball(pts);
    System.out.println(mb);
    return mb;
  }

  public final static void assertAlmostEquals(double[] expected, double[] actual)
  {
    for (int i = 0; i < expected.length; ++i)
      assertAlmostEquals(expected[i], actual[i]);
  }

  public final static void assertAlmostEquals(double expected, double actual)
  {
    assertTrue(Math.abs(expected - actual) < 1e-15);
  }
}
