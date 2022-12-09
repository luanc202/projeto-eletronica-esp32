import { NativeBaseProvider, StatusBar, Text, Center } from "native-base";

import { useFonts, Roboto_400Regular, Roboto_500Medium, Roboto_700Bold } from '@expo-google-fonts/roboto';

import Home from './src/screens/Home';
import { Loading } from './src/components/Loading'
import { THEME } from "./src/styles/theme";

export default function App() {
  const [fontsLoaded] = useFonts({ Roboto_400Regular, Roboto_500Medium, Roboto_700Bold });

  return (
    <NativeBaseProvider theme={THEME}>
      <StatusBar
        barStyle='light-content'
        backgroundColor="transparent"
        translucent />
        {fontsLoaded ? <Home /> : <Loading />}
    </NativeBaseProvider>
  );
}